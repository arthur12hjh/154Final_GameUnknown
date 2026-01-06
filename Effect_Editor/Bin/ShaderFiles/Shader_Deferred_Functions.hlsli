#ifndef SHADER_DEFERRED_FUNCTIONS
#define SHADER_DEFERRED_FUNCTIONS

#include "Engine_Shader_Defines.hlsli"
#include "Shader_Deferred_Defines.hlsli"

float FxaaLuma(float3 rgb)
{
    return rgb.y * (0.587 / 0.299) + rgb.x;
}

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
    float2 vTexcoord;
    
    vTexcoord.x = (vLightClip.x / vLightClip.w) * 0.5f + 0.5f;
    vTexcoord.y = (vLightClip.y / vLightClip.w) * -0.5f + 0.5f;

    // 텍셀 사이즈는 캐스케이드 해상도에 맞춰야 함
    float2 vTexel = 1.0f / float2(2048.0f, 2048.0f);

    // 0 ~ 1 사이로 정규화된 깊이에서 비교하니까..
    float fBias = 0.00007f;

    // 캐스케이드 밖이면 shadow 적용하지 않음
    if (vTexcoord.x < 0.0f || vTexcoord.x > 1.0f || vTexcoord.y < 0.0f || vTexcoord.y > 1.0f)
        return 1.f;
            

    vector vShadowDepth = ShadowTexure.Sample(ClampSampler, float3(vTexcoord, iSlice));

    return (vLightClip.z / vLightClip.w - fBias > vShadowDepth.x) ? 0.5f : 1.f;
}

inline float Calc_Shadow(Texture2D ShadowTexure, vector vLightClip)
{ 
    float2 vTexcoord;
    vTexcoord.x = (vLightClip.x / vLightClip.w) * 0.5f + 0.5f;
    vTexcoord.y = (vLightClip.y / vLightClip.w) * -0.5f + 0.5f;

    float fSum = 0.0f;
    float fBias = 0.00007f;
    
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
    return lerp(1.0f, 0.5f, fSum); // 그림자 강도 적용
    
    
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

inline float4 Calc_Distortion(vector vBackBuffer, texture2D SceneTexture, texture2D DistortionTexture, float2 vTexcoord)
{
    float4 vDistortion = DistortionTexture.SampleLevel(DefaultSampler, vTexcoord, 0);
    if (vDistortion.r == 0)
        return vBackBuffer;
 
    float2 vCenteredUV = vTexcoord - float2(0.5f, 0.5f);
    float fDistortionFactor = 1 + vDistortion.g * vDistortion.r * vDistortion.r;
    fDistortionFactor = clamp(fDistortionFactor, 0.2f, 2.0f);

    vCenteredUV *= fDistortionFactor;
    vCenteredUV += float2(0.5f, 0.5f);
    
    float4 vResult = SceneTexture.Sample(DefaultSampler, vCenteredUV);

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

    fResult = max(0, fIntensity - 1.45f) * 0.5; // default gThreshold = 1.26


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
    float fNdotL = saturate(dot(vNormal, vFromLight)); // 조명 쪽
    float fNdotV = saturate(dot(vNormal, vFromView)); // 카메라 쪽

    // Roughness floor (자글거림 방지)
    fRoughness = max(fRoughness, 0.4f);
    float fAlpha = max(fRoughness * fRoughness, 0.36f);
    float fK = ((fRoughness + 0.5f) * (fRoughness + 0.5f)) / 8.f;

    float fD = NDF_ggxtr(vNormal, vHalf, fAlpha);
    float fG = Geometry_Smith(vNormal, vFromView, vFromLight, fK);

    float3 vF0 = lerp(float3(0.04f, 0.04f, 0.04f), vF0Base, fMetallic);
    float3 vF = Fresnel_Schlick(saturate(dot(vHalf, vFromView)), vF0);

    float3 vNumerator = fD * fG * vF;
    float fDenom = max(4.f * fNdotL * fNdotV, 1e-7);
    float3 vSpecBRDF = vNumerator / fDenom;

    // 스페큘러 디퓨즈 줄이기
    float3 vKS = vF;

    // ===== Base Diffuse (기존 비금속 로직) =====
    //float fNonMetalFactor = 1.0f - fMetallic;
    //fNonMetalFactor *= fNonMetalFactor;
    float fNonMetalFactor = lerp(1.0f, 0.2f, fMetallic);
    
    float3 vKD = (1.f - vKS) * fNonMetalFactor;
    vKD *= 0.8f;

    // ===== Metallic diffuse floor =====
    // 금속에서도 완전히 0으로 죽지 않도록 디퓨즈를 조금 남김
    float fMetalDiffuseFloor = 0.20f; 
    float fMetalBlend = smoothstep(0.4f, 1.0f, fMetallic);

    // vKD가 0으로 수렴할수록 floor 값 쪽으로 보정
    float3 vKDMetal = lerp(vKD, fMetalDiffuseFloor.xxx, fMetalBlend);

    // 금속만 보정, 비금속은 원래 값 유지
    vKD = lerp(vKD, vKDMetal, fMetalBlend);

    // 비금속 반사 약하게 보정
    if (fMetallic < 0.1f)
    {
        vSpecBRDF *= 0.4f;
        vKD *= 1.1f;
    }

    // --------- 역광 디퓨즈 강화 (원래 코드 유지) ----------
    float fBackLight = saturate(dot(vNormal, -vFromLight)); // 뒤에서 비치는 조명
    float fBackBoost = smoothstep(0.f, 1.0f, fBackLight);
    
    float fBackBoostForDiffuse = fBackBoost * (1.0f - fMetallic);
    float fBackDiffuseBoost = lerp(1.0f, 3.f, fBackBoostForDiffuse);

    vKD *= fBackDiffuseBoost;

    // NdotL 낮은 어두운 면에서는 스펙 점점 0으로
    float fLightSideMask = smoothstep(0.1f, 0.35f, fNdotL);
    // NdotV 낮은 실루엣(가장자리)에서도 스펙 줄이기
    float fViewSideMask = smoothstep(0.1f, 0.30f, fNdotV);
    // 둘 다 만족하는 영역(= 정면에 가깝고 밝은 면)에서만 스펙 강하게
    float fSpecMask = fLightSideMask * fViewSideMask;

    // FinalSpecBoost도 이 마스크 안에서만 먹게
    float fSpecBoostMetal = lerp(1.0f, 3.0f, saturate(fMetallic));
    float fSpecBoostNonMetal = lerp(0.1f, 1.0f, fMetallic);
    float fFinalSpecBoost = lerp(fSpecBoostNonMetal, fSpecBoostMetal, fMetallic);

    float fSpecBoost = lerp(1.0f, fFinalSpecBoost, fSpecMask);

    // 기본 GGX 스펙 + 부스트 둘 다 역광/실루엣에서 같이 감쇠
    vSpecBRDF *= fSpecBoost * fSpecMask;

    // 직접광 디퓨즈
    Out.vShade = float4((vKD * vAlbedo) * (fNdotL * fAttenuation) * vLightColor, 1.f);

    // 환경광(SSAO 적용)
    float fMetalAmbFloor = 0.25f;
    float fAmbMetalMask = lerp(1.0f, fMetalAmbFloor, fMetallic);
    
    float3 vDiffuseAmb = vAlbedo * lerp(0.2f, 0.5f, 1 - fRoughness);
    vDiffuseAmb *= fAmbMetalMask * fSSAO;
    
    Out.vShade.xyz += vDiffuseAmb;

    // 직접 스페큘러
    Out.vSpecular = float4(vSpecBRDF * vLightColor * fAttenuation * fNdotL, 1.f);

    // 앰비언트 스페큘러도 같은 마스크로 줄여서 실루엣 하얀 테두리 제거
    float3 vFAmb = Fresnel_Schlick(saturate(dot(vNormal, vFromView)), vF0);
    float3 vAmbientSpec = vFAmb * lerp(0.02f, 0.08f, fMetallic) * (1 - fRoughness * fRoughness);
    vAmbientSpec *= fSpecMask;
    Out.vSpecular.xyz += vAmbientSpec;

    // (옵션) 림 스펙은 일단 꺼둔 상태 유지
    float fViewEdge = saturate(1.0f - dot(vNormal, vFromView));
    float fRimMask = (1.0f - fMetallic) * fViewEdge * fBackLight;

    float3 vRimF = Fresnel_Schlick(fViewEdge, vF0);
    float fRimIntensity = 1.5f;
    float3 vRimSpec = vRimF * fRimIntensity * fRimMask;

    //Out.vSpecular.xyz += vRimSpec * vAlbedo * fAttenuation;
    
    return Out;
}
#endif

