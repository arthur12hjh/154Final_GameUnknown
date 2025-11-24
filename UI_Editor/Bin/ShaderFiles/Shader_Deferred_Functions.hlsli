#ifndef SHADER_DEFERRED_FUNCTIONS
#define SHADER_DEFERRED_FUNCTIONS

#include "Engine_Shader_Defines.hlsli"
#include "Shader_Deferred_Defines.hlsli"

inline float4 Calc_Shadow(float4 vBackBuffer, texture2D ShadowTexture, vector vPosition)
{ 
    float2 vTexcoord;
    vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;

    float fSum = 0.0f;
    
    float2 fTexelSize = 1.0f / float2(8192.0f, 4608.0f) * 0.5f; // 그림자맵 해상도에 맞게 조정

    // PCF 3x3 샘플
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float2 offset = float2(x, y) * fTexelSize;
            float fShadowDepth = ShadowTexture.Sample(DefaultSampler, vTexcoord + offset).x * 500.0f;
            fSum += (vPosition.w - 0.1f > fShadowDepth) ? 1.0f : 0.0f;
        }
    }

    fSum /= 9.0f; // 평균 (3x3)
    vBackBuffer *= lerp(1.0f, 0.6f, fSum); // 그림자 강도 적용

    return vBackBuffer;
}

inline float4 Calc_Blur(texture2D BlurTexture, float2 vTexcoord)
{
    // 블러 연산
    float4 vBlurColor = BlurTexture.Sample(DefaultSampler, vTexcoord);
    
    return vBlurColor;
}

inline float4 Calc_Glow(texture2D GlowTexture, float2 vTexcoord)
{
    // 블러 연산
    float4 vGlowColor = GlowTexture.Sample(DefaultSampler, vTexcoord);
    
    return vGlowColor;
}

/* 배럴 왜곡 이용해서 구현한거임. 내부 로직 바꾸고 싶으면 일단 디코로 따로 말해줘*/
inline float4 Calc_Distortion(vector vBackBuffer, texture2D SceneTexture, texture2D DistortionTexture, float2 vTexcoord)
{       
    /* 디스토션 렌더타겟 텍스쳐 샘플링 */
    float4 vDistortion = DistortionTexture.Sample(DefaultSampler, vTexcoord);
    
    /* r 성분을 디스토션할 성분으로 잡아놨으니까, r이 0이라면 리턴. */
    if(vDistortion.r == 0)
        return vBackBuffer;
 
    // 0 ~ 1 -> -0.5 ~ 0.5
    float2 vCenteredUV = vTexcoord - float2(0.5f, 0.5f);
    float fDistortionFactor = 1 + vDistortion.g * vDistortion.r * vDistortion.r;
    //너무 낮은값 쓰면 찢어지는 효과 남. (같은 픽셀 계속 디스토션 돌려서)
    fDistortionFactor = clamp(fDistortionFactor, 0.2f, 2.0f);

    vCenteredUV *= fDistortionFactor;
    vCenteredUV += float2(0.5f, 0.5f);
    
    float4 vResult = SceneTexture.Sample(MirrorSampler, vCenteredUV);

    // 원래 색과 섞어.
    //float fFade = (vDistortion.r > 0.0f) ? 1.0f : 0.0f;
    //vResult.rgb = lerp(vScene.rgb, vResult.rgb, 0.8f * fFade);
    
    return vResult;
}

float4 Calc_Fog(vector vBackBuffer, texture2D FogTexture, vector vFogColor, float2 vTexCoord)
{
    float fFogPower = FogTexture.Sample(DefaultSampler, vTexCoord);
    
    return fFogPower * vBackBuffer + (1 - fFogPower) * vFogColor;
}

/* 블룸 커브 수식 3개. HALO3 에서 나왔다는데 일단 가져옴..*/
float GetBloomCurve(float fIntensity)
{
    float fResult = fIntensity;
    fIntensity *= 2.0f;

    fResult = max(0, fIntensity - 1.4f) * 0.5; // default gThreshold = 1.26


    return fResult * 0.5f;
}

float NDF_ggxtr(float3 vNormal, float3 vHalfWayVector, float fAlpha) // NormalDistributionGGXTR, (H, halfWay vector), (A, Roughness), (N, Normal)
{
    float a2 = fAlpha * fAlpha;
    float NdotH = saturate(dot(vNormal, vHalfWayVector));
    float NdotH2 = NdotH * NdotH;
    
    float nom = a2;
    float fDenom = (NdotH2 * (a2 - 1.f) + 1.f);
    
    fDenom = 3.14f * fDenom * fDenom;
    return (nom / fDenom);
}

// F->G
// GschlickGGX(n, v, k) 
// k는 이하 두개 조명 조건에 따라 가변적임
// Direct //  Kdir -> ((a + 1) * (a + 1)) / 8 // direct lighting 추천
// IBL Lighting // Kibl -> a * a / 2 // 이미지 기반 조명기법

float Geometry_SchlickGGX(float NdotV, float fK)
{
    return NdotV / (NdotV * (1.0f - fK) + fK);
}

float Geometry_Smith(float3 vNormal, float3 vFromView, float3 vFromLight, float k)
{
    // Geometry Obstruction     
    // 철이 있는, 주로 튀어나온 장애물
    float NdotV = saturate(dot(vNormal, vFromView));
    
    // Geometry Shadowing       
    // 요가 있는, 주로 음푹 들어간 장애물 (그래서 셰도잉)
    float NdotL = saturate(dot(vNormal, vFromLight));

    // 요철이 골고루 있다고 가정하고 적당히 섞음
    return Geometry_SchlickGGX(NdotV, k) * Geometry_SchlickGGX(NdotL, k);
}

// F -> F
float3 Fresnel_Schlick(float cosTheta, float3 F0)
{
    // 모서리 부분의 반사
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

PS_OUT_LIGHT PBR_Light(float3 vNormal, float3 vFromView, float3 vFromLight,
    float3 vAlbedo, float fMetallic, float fRoughness,
    float3 vLightColor, float fAttenuation, float3 vFO, float fSSAO)
{
    PS_OUT_LIGHT Out;
    Out.vShade = 0;
    Out.vSpecular = 0;
    
    float3 vHalfVector = normalize(vFromView + vFromLight);
    float NdotL = saturate(dot(vNormal, vFromLight));
    float NdotV = saturate(dot(vNormal, vFromView));
    
    if (NdotL <= 0 || NdotV <= 0)
    {
        clamp(NdotL, 0.f, 1.f);
        clamp(NdotV, 0.f, 1.f);
    }
    
    float fAlpha = max(fRoughness * fRoughness, 0.04f);
    float k = ((fRoughness + 1.f) * (fRoughness + 1.f)) / 8.f;
    
    float D = NDF_ggxtr(vNormal, vHalfVector, fAlpha);
    float G = Geometry_Smith(vNormal, vFromView, vFromLight, k);
    float3 F = Fresnel_Schlick(saturate(dot(vHalfVector, vFromView)), vFO);
    
    float3 vNumerator = D * G * F;
    float fDenom = max(4.f * NdotL * NdotV, 1e-7);
    float3 specularBRDF = vNumerator / fDenom;
    
    float3 kS = F;
    // 금속이여도 일부 디퓨즈 가질 수 있게 세팅
    float3 kD = (1.f - kS) * max((1.f - fMetallic), 0.2f);
    
    // 직접광 연산
    Out.vShade = float4((kD * vAlbedo) * (NdotL * fAttenuation) * vLightColor, 1.f);
    // 환경광 연산
    float3 vDiffuseAmbient = vAlbedo * lerp(0.04f, 0.25f, 1 - fRoughness);
    vDiffuseAmbient *= (1 - fMetallic) * fSSAO;
    Out.vShade.xyz += vDiffuseAmbient;
    
    Out.vSpecular = float4(specularBRDF * vLightColor * fAttenuation * NdotL, 1.f);
    F = Fresnel_Schlick(saturate(dot(vNormal, vFromView)), vFO);
    float3 vAmbientSpec = F * 0.02f * (1 - fRoughness * 0.7f);
    Out.vSpecular.xyz += vAmbientSpec;
    
    
    return Out;
}

#endif
