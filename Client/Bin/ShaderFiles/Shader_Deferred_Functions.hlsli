#include "Engine_Shader_Defines.hlsli"

uint MAX_CASCADE_NUM = 3;

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

inline float3 Calc_Shadow3x3(Texture2DArray ShadowTexture, SamplerComparisonState samplerComp, float ShadowConst[3], float4x4 MatShadowVP[3], float4 vPosition, float Dist)
{
    float3 vShadowColor = float3(1.f, 1.f, 1.f);
    uint iIndex = MAX_CASCADE_NUM - 1;
    for (uint i = 0; i < MAX_CASCADE_NUM; ++i)
    {
        if (Dist <= ShadowConst[MAX_CASCADE_NUM])
        {
            iIndex = i;
            break;
        }
    }
    
    float4 vPosShadowSpace = mul(vPosition, MatShadowVP[iIndex]);
    float3 vTexcoord = vPosShadowSpace.xyz / vPosShadowSpace.w;   
    
    float cmp_z = vTexcoord.z;
    float litSum = 0;
    
    //// PCF 3x3 샘플
    //// 주변 픽셀을 가져와서 랜더링
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            litSum += ShadowTexture.SampleCmpLevelZero(ShadowSampler, float3(vTexcoord.xy, iIndex), cmp_z, int2(x, y));
        }
    }
   
    float shadowValue = litSum / 9.0f;
    vShadowColor = lerp(float3(0.f, 0.f, 0.f), float3(1.f,1.f,1.f), shadowValue); // 그림자 강도 적용
    return vShadowColor;
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

/* 블룸 커브 수식 3개. HALO 책에서 나왔다는데 일단 가져옴..*/
float GetBloomCurve(float fIntensity)
{
    float fResult = fIntensity;
    fIntensity *= 2.0f;

    /* method 1 */
    fResult = fIntensity * 0.05 + max(0, fIntensity - 1.4f) * 0.5; // default gThreshold = 1.26

    /* method 2 */
    //result = x * x / 3.2;

    /* method 3 */
    //fResult = max(0, fIntensity - 0.6f); // default gThreshold = 1.0
    //fResult *= fResult;

    return fResult * 0.5f;
}

float3 Fresnel_Shlick(in float3 f0, in float3 f90, in float x)
{
    return f0 + (f90 - f0) * pow(1.f - x, 5.f);
}

float Diffuse_Burley(in float NdotL, in float NdotV, in float LdotH, in float roughness)
{
    float fd90 = 0.5f + 2.f * roughness * LdotH * LdotH;
    return Fresnel_Shlick(1, fd90, NdotL).x * Fresnel_Shlick(1, fd90, NdotV).x;
}

float Specular_D_GGX(in float alpha, in float NdotH)
{
    const float alpha2 = alpha * alpha;
    const float lower = (NdotH * NdotH * (alpha2 - 1)) + 1;
    return alpha2 / max(1e-6f, 3.14f * lower * lower);
}

float G_Shlick_Smith_Hable(float alpha, float LdotH)
{
    return rcp(lerp(LdotH * LdotH, 1, alpha * alpha * 0.25f));
}

float3 Specular_BRDF(in float alpha, in float3 specularColor, in float NdotV, in float NdotL, in float LdotH, in float NdotH)
{
    float specular_D = Specular_D_GGX(alpha, NdotH);

    float3 specular_F = Fresnel_Shlick(specularColor, 1, LdotH);

    float specular_G = G_Shlick_Smith_Hable(alpha, LdotH);

    return specular_D * specular_F * specular_G;
}

float3 LightSurface(
    in float3 V, in float3 N, in float3 lightColor, in float3 lightDirection, in float3 albedo, in float roughness, in float metallic, in float ambientOcclusion)
{
    const float kSpecularCoefficient = 0.04;
    const float NdotV = saturate(dot(N, V));
    const float alpha = roughness * roughness;

    const float3 c_diff = lerp(albedo, float3(0, 0, 0), metallic) * ambientOcclusion;
    const float3 c_spec = lerp(kSpecularCoefficient, albedo, metallic) * ambientOcclusion;

    float3 acc_color = 0;

    const float3 L = normalize(-lightDirection);

    const float3 H = normalize(L + V);

    const float NdotL = saturate(dot(N, L));
    const float LdotH = saturate(dot(L, H));
    const float NdotH = saturate(dot(N, H));

    float diffuse_factor = Diffuse_Burley(NdotL, NdotV, LdotH, roughness);
    float3 specular = Specular_BRDF(alpha, c_spec, NdotV, NdotL, LdotH, NdotH);

    acc_color += NdotL * lightColor * (((c_diff * diffuse_factor) + specular));
    acc_color += c_diff * float3(0.5f, 0.5f, 0.5f);
    acc_color += c_spec * float3(0.5f, 0.5f, 0.5f);

    return acc_color;
}
