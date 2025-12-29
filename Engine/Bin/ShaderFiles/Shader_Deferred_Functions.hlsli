#ifndef SHADER_DEFERRED_FUNCTIONS
#define SHADER_DEFERRED_FUNCTIONS

#include "Engine_Shader_Defines.hlsli"
#include "Shader_Deferred_Defines.hlsli"

float4 GetWorldPosition(texture2D DepthTexture, float fFar, float2 vTexcoord, matrix ProjMatrixInv, matrix ViewMatrixInv)
{
    // 좌표 복구
    float4 vDepthDesc = DepthTexture.Sample(DefaultSampler, vTexcoord);
    float fViewZ = vDepthDesc.y * fFar;

    /* 좌표변환은 틀린거 없고.. */
    /* 로컬위치 * 월드 * 뷰 * 투영 / w */
    vector vPosition;
    vPosition.x = vTexcoord.x * 2.f - 1.f;
    vPosition.y = vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    /* 로컬위치 * 월드 * 뷰 * 투영  */
    vPosition = vPosition * fViewZ;
    /* 로컬위치 * 월드 * 뷰  */
    vPosition = mul(vPosition, ProjMatrixInv);
    /* 로컬위치 * 월드   */
    vPosition = mul(vPosition, ViewMatrixInv);
    
    return vPosition;
}

float4 GetViewPosition(texture2D DepthTexture, float fFar, float2 vTexcoord, matrix ProjMatrixInv)
{
    // 좌표 복구
    float4 vDepthDesc = DepthTexture.Sample(DefaultSampler, vTexcoord);
    float fViewZ = vDepthDesc.y * fFar;

    /* 좌표변환은 틀린거 없고.. */
    /* 로컬위치 * 월드 * 뷰 * 투영 / w */
    vector vPosition;
    vPosition.x = vTexcoord.x * 2.f - 1.f;
    vPosition.y = vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    /* 로컬위치 * 월드 * 뷰 * 투영  */
    vPosition = vPosition * fViewZ;
    /* 로컬위치 * 월드 * 뷰  */
    vPosition = mul(vPosition, ProjMatrixInv);
    
    return vPosition;
}

inline float BayerDither(float2 pixelPos)
{
    uint x = (uint) pixelPos.x & 3; // % 4
    uint y = (uint) pixelPos.y & 3; // % 4

    uint v = Bayer4x4[y][x]; // 0 ~ 15
    return (v + 0.5f) / 16.0f; // 0 ~ 1 사이 값
}

inline float Calc_Shadow_CSM(Texture2DArray ShadowTexure, vector vLightClip, uint iSlice)
{
    /*
        float fSum = 0.0f;
    
    float2 vTexcoord;
    
    vTexcoord.x = (vLightClip.x / vLightClip.w) * 0.5f + 0.5f;
    vTexcoord.y = (vLightClip.y / vLightClip.w) * -0.5f + 0.5f;

    // 텍셀 사이즈는 캐스케이드 해상도에 맞춰야 함
    float2 vTexel = 1.0f / float2(2048.0f, 2048.0f);

    // 0 ~ 1 사이로 정규화된 깊이에서 비교하니까..
    float fBias = 0.0002f;

    // 캐스케이드 밖이면 shadow 적용하지 않음
    if (vTexcoord.x < 0.0f || vTexcoord.x > 1.0f || vTexcoord.y < 0.0f || vTexcoord.y > 1.0f)
        return 1.f;

    [unroll]
    for (int iX = -1; iX <= 1; ++iX)
    {
        [unroll]
        for (int iY = -1; iY <= 1; ++iY)
        {
            float2 vOffset = float2(iX, iY) * vTexel;
            vector vShadowDepth = ShadowTexure.Sample(ClampSampler, float3(vTexcoord + vOffset, iSlice));
            // 투영행렬까지만 곱했다면 w에 뷰스페이스 상의 z값 남아있을거고,
            // 그림자엔 Far로 정규화한 0~1사이 값인 뷰 스페이스 상의 z가 있으니까 얠 다시 far 곱해서 연산. 
            fSum += (vLightClip.z / vLightClip.w - fBias > vShadowDepth.x) ? 1.0f : 0.0f;
        }
    }

    fSum /= 9.0f;
    return lerp(1.0f, 0.6f, fSum);
    */
   
    float2 vTexcoord;
    
    vTexcoord.x = (vLightClip.x / vLightClip.w) * 0.5f + 0.5f;
    vTexcoord.y = (vLightClip.y / vLightClip.w) * -0.5f + 0.5f;

    // 텍셀 사이즈는 캐스케이드 해상도에 맞춰야 함
    float2 vTexel = 1.0f / float2(2048.0f, 2048.0f);

    // 0 ~ 1 사이로 정규화된 깊이에서 비교하니까..
    float fBias = 0.0002f;

    // 캐스케이드 밖이면 shadow 적용하지 않음
    if (vTexcoord.x < 0.0f || vTexcoord.x > 1.0f || vTexcoord.y < 0.0f || vTexcoord.y > 1.0f)
        return 1.f;
            

    vector vShadowDepth = ShadowTexure.Sample(ClampSampler, float3(vTexcoord, iSlice));

    return (vLightClip.z / vLightClip.w - fBias > vShadowDepth.x) ? 0.6f : 1.f;
}

inline float Calc_Shadow(Texture2D ShadowTexure, vector vLightClip)
{ 
    float2 vTexcoord;
    vTexcoord.x = (vLightClip.x / vLightClip.w) * 0.5f + 0.5f;
    vTexcoord.y = (vLightClip.y / vLightClip.w) * -0.5f + 0.5f;

    float fSum = 0.0f;
    float fBias = 0.002f;
    
    if (vTexcoord.x < 0 || vTexcoord.x > 1 || vTexcoord.y < 0 || vTexcoord.y > 1)
        return 1.0f;
    
    //_uint2 m_vStaticShadowMapSize = { 16384, 8192 };
    float2 fTexelSize = 1.0f / float2(16384.f, 16384.f) * 2.f; // 그림자맵 해상도에 맞게 조정

    // PCF 3x3 샘플
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float2 vOffset = float2(x, y) * fTexelSize;
            vector vShadowDepth = ShadowTexure.Sample(ClampSampler, float2(vTexcoord + vOffset));
            fSum += (vLightClip.z / vLightClip.w - fBias > vShadowDepth.x) ? 1.0f : 0.0f;
        }
    }
    
    fSum /= 9.0f; // 평균 (3x3)
    return lerp(1.0f, 0.6f, fSum); // 그림자 강도 적용
    
    
    //################################# NONE PCF
    /*
        float2 vTexcoord;
    vTexcoord.x = (vLightClip.x / vLightClip.w) * 0.5f + 0.5f;
    vTexcoord.y = (vLightClip.y / vLightClip.w) * -0.5f + 0.5f;

    float fBias = 0.0002f;
    
    if (vTexcoord.x < 0 || vTexcoord.x > 1 || vTexcoord.y < 0 || vTexcoord.y > 1)
        return 1.0f;

    vector vShadowDepth = ShadowTexure.Sample(DefaultSampler, float2(vTexcoord));
    
    return (vLightClip.z / vLightClip.w - fBias > vShadowDepth.x) ? 0.6f : 1.f;
    */
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

    // 원래 색과 섞음.
    //float fFade = (vDistortion.r > 0.0f) ? 1.0f : 0.0f;
    //vResult.rgb = lerp(vScene.rgb, vResult.rgb, 0.8f * fFade);
    
    return vResult;
}

float4 Calc_Fog(vector vBackBuffer, texture2D FogTexture, vector vFogColor, float2 vTexCoord)
{
    float fFogPower = FogTexture.Sample(DefaultSampler, vTexCoord);
    
    return fFogPower * vBackBuffer + (1 - fFogPower) * vFogColor;
}

float4 Calc_VolumeFog(vector vBackBuffer, texture2D FogTexture , float2 vTexCoord)
{
    float4 vFog = FogTexture.Sample(DefaultSampler, vTexCoord);
    float3 vResult = vBackBuffer.rgb * vFog.a + vFog.rgb;
    
    return float4(vResult, vBackBuffer.a);
}

/* 블룸 커브 수식 3개. HALO3 에서 나왔다는데 일단 가져옴..*/
float GetBloomCurve(float fIntensity)
{
    float fResult = fIntensity;
    fIntensity *= 2.0f;

    fResult = max(0, fIntensity - 1.0f) * 0.5; // default gThreshold = 1.26


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

PS_OUT_LIGHT PBR_Light(
    float3 vNormal, float3 vFromView, float3 vFromLight,
    float3 vAlbedo, float fMetallic, float fRoughness,
    float3 vLightColor, float fAttenuation, float3 vF0Base, float fSSAO)
{
    PS_OUT_LIGHT Out;
    Out.vShade = 0;
    Out.vSpecular = 0;
    
    float3 vHalf = normalize(vFromView + vFromLight);
    float fNdotL = saturate(dot(vNormal, vFromLight));
    float fNdotV = saturate(dot(vNormal, vFromView));

    // Roughness floor (자글거림 방지)
    fRoughness = max(fRoughness, 0.6f);
    float fAlpha = max(fRoughness * fRoughness, 0.36f);
    float fK = ((fRoughness + 0.5f) * (fRoughness + 0.5f)) / 8.f;

    float fD = NDF_ggxtr(vNormal, vHalf, fAlpha);
    float fG = Geometry_Smith(vNormal, vFromView, vFromLight, fK);

    // 금속/비금속에 따른 F0
    float3 vF0 = lerp(float3(0.04f, 0.04f, 0.04f), vF0Base, fMetallic);
    float3 vF = Fresnel_Schlick(saturate(dot(vHalf, vFromView)), vF0);

    float3 vNumerator = fD * fG * vF;
    float fDenom = max(4.f * fNdotL * fNdotV, 1e-7);
    float3 vSpecBRDF = vNumerator / fDenom;

    // 스페큘러 반사광 강화
    // 금속은 매우 세게, 비금속은 약하게
    float fSpecBoostMetal = lerp(1.0f, 4.0f, saturate(fMetallic)); // 금속 → +4x
    float fSpecBoostNonMetal = lerp(0.1f, 1.0f, fMetallic); // 비금속 → 60% 정도만 유지

    // 비금속이면 약하게, 금속이면 매우 강하게
    float fFinalSpecBoost = lerp(fSpecBoostNonMetal, fSpecBoostMetal, fMetallic);
    vSpecBRDF *= fFinalSpecBoost;

    // 하이라이트가 너무 퍼지지 않도록 살짝 선명하게
    //vSpecBRDF = pow(vSpecBRDF, lerp(1.0f, 0.8f, fMetallic));

    // 스페큘러 디퓨즈 줄이기
    // 기존 kD를 전체적으로 약하게
    float3 vKS = vF;
    float3 vKD = (1.f - vKS) * (1.f - fMetallic);

    // 디퓨즈 자체 약화 (스페큘러가 강해질 때 과충돌 방지)
    vKD *= 0.8f;

    // 비금속 반사 약하게
    if (fMetallic < 0.1f)
    {
        vSpecBRDF *= 0.4f; // 비금속 → 반사광 약화
        vKD *= 1.1f; // 반사 줄었으니 알베도 조금 보정
    }

    // 역광 디퓨즈 강화
    float fBackLight = saturate(dot(vNormal, -vFromLight)); // 뒤에서 비치는 조명
    float fBackBoost = smoothstep(0.f, 1.0f, fBackLight);
    
    // 역광일 때 vKD * Albedo 를 살짝 부스트
    float fBackDiffuseBoost = lerp(1.0f, 3.f, fBackBoost);
    vKD *= fBackDiffuseBoost;

    // 직접광
    Out.vShade = float4((vKD * vAlbedo) * (fNdotL * fAttenuation) * vLightColor, 1.f);

    // 환경광(SSAO 적용)
    float3 vDiffuseAmb = vAlbedo * lerp(0.2f, 0.5f, 1 - fRoughness);
    vDiffuseAmb *= (1 - fMetallic) * fSSAO;
    Out.vShade.xyz += vDiffuseAmb;

    // 직접 스페큘러
    Out.vSpecular = float4(vSpecBRDF * vLightColor * fAttenuation * fNdotL, 1.f);

    // 앰비언트 스페큘러
    float3 vFAmb = Fresnel_Schlick(saturate(dot(vNormal, vFromView)), vF0);
    float3 vAmbientSpec = vFAmb * lerp(0.02f, 0.08f, fMetallic) * (1 - fRoughness * fRoughness);
    Out.vSpecular.xyz += vAmbientSpec;

     
    //위에서 계산한 역광 항 이용
    float fViewEdge = saturate(1.0f - dot(vNormal, vFromView)); // 실루엣일수록 강하게

    // 금속만 림라이트 효과 적용
    float fMetalRim = fMetallic * fBackLight * fViewEdge;

    // 림라이트 스펙큘러 색: Fresnel 기반
    float3 vRimF = Fresnel_Schlick(fViewEdge, vF0);

    // 강도 조절
    float fRimIntensity = 1.5f; // 원하는만큼 조절
    float3 vRimSpec = vRimF * fRimIntensity * fMetalRim;

    // 기존 스페큘러에 add
    Out.vSpecular.xyz += vRimSpec * vAlbedo * fAttenuation;
    
    return Out;
}
#endif
