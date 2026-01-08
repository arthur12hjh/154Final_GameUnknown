#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;
float g_fFar;
//안개 색은 렌더타겟에 세팅해놓는다.
float4 g_vFogColor;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

matrix g_StaticLightViewMatrix;
matrix g_StaticLightProjMatrix;
matrix g_LightViewMatrix[5];
matrix g_LightProjMatrix[5];

texture2D g_Texture;
vector g_vCamPosition;
float g_fDensity;
float g_fStepSize;
float g_fVolumetricG;
float g_fHDRExposure; 

texture2D g_NormalTexture;
texture2D g_DiffuseTexture;
texture2D g_ShadeTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;
texture2D g_ORMTexture;
texture2D g_ShadowTexture;
texture2D g_StaticShadowTexture;
texture2D g_SSAOTexture;
texture2D g_SSSAOTexture;

texture2D g_BlackBlendTexture;
texture2D g_BlurFinalTexture;
texture2D g_GlowFinalTexture;
texture2D g_GlowBloomTexture;
texture2D g_MetaballTexture;
texture2D g_EmissiveFinalTexture;

texture2D g_DistortionTexture;
texture2D g_FogTexture;
texture2D g_VolumeFogTexture;
texture2D g_BloomTexture;
texture2D g_BloomSceneTexture;
texture2D g_VolumetricTexture;
texture2D g_SceneTexture;
texture2D g_ScreenTexture;

texture2D g_ShadowBlurXTexture;
texture2D g_ShadowAreaTexture; 
Texture2DArray g_CascadeShadowTexture;

float g_fCascadeEnds[6] = { 
    0.1f,
	6.0f,
	20.0f,
	40.f,
	100.f,
	500.f,
};


vector g_vLightDir;
vector g_vLightPos;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;
float g_fLightRange;
float g_fFalloff;
float g_fTheta;
float g_fPhi;

#define FXAA_EDGE_THRESHOLD      (1.0/8.0)
#define FXAA_EDGE_THRESHOLD_MIN  (1.0/24.0)
#define FXAA_SEARCH_STEPS        32
#define FXAA_SEARCH_ACCELERATION 1
#define FXAA_SEARCH_THRESHOLD    (1.0/4.0)
#define FXAA_SUBPIX              1
#define FXAA_SUBPIX_FASTER       0
#define FXAA_SUBPIX_CAP          (3.0/4.0)
#define FXAA_SUBPIX_TRIM         (1.0/4.0)
#define FXAA_SUBPIX_TRIM_SCALE (1.0/(1.0 - FXAA_SUBPIX_TRIM))

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;   
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);   
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{                                                                                                                                                                                                                                                                      
    PS_OUT_BACKBUFFER Out;
    
    Out.vBackBuffer = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;   
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;

    float4 vORMDesc = g_ORMTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vAlbedo = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    // Normal Strength 증가 --------------------------------------------

    float3 N = normalize(vNormalDesc.xyz * 2.f - 1.f);
    float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fFar;

    /* 로컬위치 * 월드 * 뷰 * 투영 / w */
    vector vPosition;
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    
    /* 로컬위치 * 월드 * 뷰 * 투영  */
    vPosition = vPosition * fViewZ;
    /* 로컬위치 * 월드 * 뷰  */
    vPosition = mul(vPosition, g_ProjMatrixInv);
    /* 로컬위치 * 월드   */
    vPosition = mul(vPosition, g_ViewMatrixInv);

    float3 V = normalize((vPosition - g_vCamPosition).xyz);
    float3 L = normalize(g_vLightDir.xyz) * -1.f;

    float AO = vORMDesc.r;
    float Rough = vORMDesc.g;
    float MetalSrc = vORMDesc.b;
    float A = vORMDesc.a;

    float Metallic = 0.f;
    float3 F0 = float3(0.04f, 0.04f, 0.04f);

    bool isPBR = (AO != 0.f || Rough != 0.f || MetalSrc != 0.f || A != 0.f);
    bool isORSS = isPBR && (A > 0.f);
    bool isORM = isPBR && (A == 0.f);

    // Phong
    if (!isPBR)
    {
        float NdotL = saturate(dot(N, L));

        float3 diff = vAlbedo.rgb * g_vLightDiffuse.rgb * saturate(NdotL + (g_vLightAmbient.rgb * g_vMtrlAmbient.rgb));

        float3 refl = reflect(-L, N);
        float3 spec = (g_vLightSpecular.rgb * g_vMtrlSpecular.rgb) * pow(max(dot(-V, normalize(refl)), 0.f), 50.f);

        Out.vShade = float4(diff, 1);
        Out.vSpecular = float4(spec, 1);
        //Phong 셰이딩은 여기서 끝
        return Out;
    }

    // ORSS(피부)
    if (isORSS)
    {
        AO = vORMDesc.r;
        Rough = vORMDesc.g;
        float specFactor = saturate(vORMDesc.b);
        Metallic = 0.f;

        float3 baseF0 = float3(0.028f, 0.028f, 0.028f);
        F0 = baseF0 + specFactor * 0.05f;
    }
    // ORM (orm 마스크) 
    else if (isORM)
    {
        AO = vORMDesc.r;
        Rough = vORMDesc.g;
        Metallic = saturate(vORMDesc.b);
        F0 = lerp(float3(0.04f, 0.04f, 0.04f), vAlbedo.rgb, Metallic);
    }

    float AOStr = lerp(1.0f, 1.2f, AO);
    Out = PBR_Light(N, -V, L, vAlbedo.rgb, Metallic, Rough, vDepthDesc.z != 1 ? g_vLightDiffuse.xyz : float3(0.5f, 0.5f, 0.5f), 1.f, F0, 1.f);
    Out.vShade.rgb *= AOStr;

    // ORSS라면, SSS 추가
    if (isORSS)
    {
        float3 base = Out.vShade.rgb;

        float luma = dot(base, float3(0.299f, 0.587f, 0.114f));
        float3 bloodHue = float3(1.0f, 0.7f, 0.7f);
        float3 bloodColor = bloodHue * luma;

        float ndl = saturate(dot(N, L));
        float edge = pow(1.0f - ndl, 1.2f);
        float sss = saturate(A * g_SSSAOTexture.Sample(DefaultSampler, In.vTexcoord).g);
        float w = saturate(sss * edge * 0.35f);

        float3 result = lerp(base, bloodColor, w);
        Out.vShade.rgb = result;

        // 역광 SSS BackScatter
        float back = saturate(dot(-N, L));
        float3 backSSS = vAlbedo.rgb * float3(1.0f, 0.7f, 0.7f) * pow(back, 1.1f) * 0.25f;

        Out.vShade.rgb += backSSS;
    }
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;

    float4 vORMDesc = g_ORMTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vAlbedo = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fFar;

    
    float3 N = normalize(vNormalDesc.xyz * 2.f - 1.f);
    /* 로컬위치 * 월드 * 뷰 * 투영 / w */
    vector vPosition;
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    
    /* 로컬위치 * 월드 * 뷰 * 투영  */
    vPosition = vPosition * fViewZ;
    /* 로컬위치 * 월드 * 뷰  */
    vPosition = mul(vPosition, g_ProjMatrixInv);
    /* 로컬위치 * 월드   */
    vPosition = mul(vPosition, g_ViewMatrixInv);
    
    float3 V = normalize((vPosition - g_vCamPosition).xyz);
    vector LOrigin = (vPosition - g_vLightPos) * -1.f;
    vector L = normalize(LOrigin);
    
    float fDistance = length(LOrigin);
    
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    float NdotL = saturate(dot(N, L.xyz));
    
    float AO = vORMDesc.r;
    float Rough = vORMDesc.g;
    float MetalSrc = vORMDesc.b;
    float A = vORMDesc.a;

    float Metallic = 0.f;
    float3 F0 = float3(0.04f, 0.04f, 0.04f);

    bool isPBR = (AO != 0.f || Rough != 0.f || MetalSrc != 0.f || A != 0.f);
    bool isORSS = isPBR && (A > 0.f);
    bool isORM = isPBR && (A == 0.f);

    // Phong
    if (!isPBR)
    {
        float fNdotL = dot(N, L.xyz);
        float3 vReflect = reflect(-L.xyz, N);
   
        Out.vShade = fAtt * (g_vLightDiffuse * saturate(max(fNdotL, 0.f) + (g_vLightAmbient * g_vMtrlAmbient)));
        Out.vSpecular = fAtt * ((g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(V * -1.f, normalize(vReflect)), 0.f), 50.f));
    
        return Out;
    }

    // ORSS(피부)
    if (isORSS)
    {
        AO = vORMDesc.r;
        Rough = vORMDesc.g;
        float specFactor = saturate(vORMDesc.b);
        Metallic = 0.f;

        float3 baseF0 = float3(0.028f, 0.028f, 0.028f);
        F0 = baseF0 + specFactor * 0.05f;
    }
    // ORM (orm 마스크) 
    else if (isORM)
    {
        AO = vORMDesc.r;
        Rough = vORMDesc.g;
        Metallic = saturate(vORMDesc.b);
        F0 = lerp(float3(0.04f, 0.04f, 0.04f), vAlbedo.rgb, Metallic);
    }

    float AOStr = lerp(1.0f, 1.2f, AO);
    Out = PBR_Light(N, -V, L.xyz, vAlbedo.rgb, Metallic, Rough, g_vLightDiffuse.xyz, 1.f, F0, 1.f);
    
    Out.vShade *= fAtt;
    Out.vSpecular *= fAtt;
    
    Out.vShade.rgb *= AOStr;

    //점조명은 ORSS 적용 X
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    float4 vAlbedo = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fFar;
    
    float4 vORMDesc = g_ORMTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vPosition;
    
    /* 로컬위치 * 월드 * 뷰 * 투영 / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    
    /* 로컬위치 * 월드 * 뷰 * 투영  */
    vPosition = vPosition * fViewZ;
    
    /* 로컬위치 * 월드 * 뷰  */
    vPosition = mul(vPosition, g_ProjMatrixInv);
    
    /* 로컬위치 * 월드   */
    vPosition = mul(vPosition, g_ViewMatrixInv);
    
    vector vLightToPixel = vPosition - g_vLightPos;
    float fDistance = length(vLightToPixel);
    
    // 노멀벡터
    float3 N = normalize(vNormalDesc.xyz * 2.f - 1.f);
    // 카메라 -> 위치
    float3 V = normalize((vPosition - g_vCamPosition).xyz);
    // 위치 -> 빛
    vector LOrigin = (vPosition - g_vLightPos) * -1.f;
    // 정규화한 위치 -> 빛 벡터
    vector L = normalize(LOrigin);
    
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    
    // 각도 감쇠
    vector vLightDir = normalize(vLightToPixel) * -1.f;
    vector vSpotLightDir = normalize(g_vLightDir);
    
    // 픽셀이 중심축에서 얼마나 벗어났는지
    float fCosAngle = dot(vLightDir, vSpotLightDir);
    
    float fSpotAtt = 0.f;
    
    // 픽셀이 외부 원뿔(Phi)내에 있는 경우에만 빛 계산
    if(fCosAngle > cos(g_fPhi))
    {
        // 픽셀이 내부 원뿔(Theta)내에 있는 경우 -> 최대 밝기
        if (fCosAngle > cos(g_fTheta))
        {
            fSpotAtt = 1.f;
        }
        // 픽셀이 내부와 외부 원뿔 사이에 있는 경우 -> fFalloff 적용
        else
        {
            // 각도에 따른 선형보간 (FallOff)
            fSpotAtt = pow(saturate((fCosAngle - cos(g_fPhi)) / (cos(g_fTheta) - cos(g_fPhi))), g_fFalloff);

        }
    }
    
    float fFinalAtt = fAtt * fSpotAtt;
    
    //pbr 적용하면서 변수이름좀 바꿧습니다 
    /*    
    vector vSurfaceNormal = normalize(vNormal);
    vector vViewDir = normalize(vPosition - g_vCamPosition) * -1.f;
    vector vReflect = reflect(vLightDir, vSurfaceNormal);   
    */ 
  
    float AO = vORMDesc.r;
    float Rough = vORMDesc.g;
    float MetalSrc = vORMDesc.b;
    float A = vORMDesc.a;

    float Metallic = 0.f;
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    
    bool isPBR = (AO != 0.f || Rough != 0.f || MetalSrc != 0.f || A != 0.f);
    bool isORSS = isPBR && (A > 0.f);
    bool isORM = isPBR && (A == 0.f);

    // Phong
    if (!isPBR)
    {
        float fNdotL = dot(N, L.xyz);
        float3 vReflect = reflect(-L.xyz, N);
   
        Out.vShade = fFinalAtt * (g_vLightDiffuse * saturate(max(fNdotL, 0.f) + (g_vLightAmbient * g_vMtrlAmbient)));
        Out.vSpecular = fFinalAtt * ((g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(V * -1.f, normalize(vReflect)), 0.f), 50.f));
    
        return Out;
    }

    // ORSS(피부)
    if (isORSS)
    {
        AO = vORMDesc.r;
        Rough = vORMDesc.g;
        float specFactor = saturate(vORMDesc.b);
        Metallic = 0.f;

        float3 baseF0 = float3(0.028f, 0.028f, 0.028f);
        F0 = baseF0 + specFactor * 0.05f;
    }
    // ORM (orm 마스크) 
    else if (isORM)
    {
        AO = vORMDesc.r;
        Rough = vORMDesc.g;
        Metallic = saturate(vORMDesc.b);
        F0 = lerp(float3(0.04f, 0.04f, 0.04f), vAlbedo.rgb, Metallic);
    }

    float AOStr = lerp(1.0f, 1.2f, AO);
    Out = PBR_Light(N, -V, L.xyz, vAlbedo.rgb, Metallic, Rough, g_vLightDiffuse.xyz, 1.f, F0, 1.f);
    
    Out.vShade *= fFinalAtt;
    Out.vSpecular *= fFinalAtt;
    
    Out.vShade.rgb *= AOStr;
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_VOLUMETRIC_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    Out.vShade = float4(0.f, 0.f, 0.f, 0.f);
    Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_VOLUMETRIC_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    Out.vShade = float4(0.f, 0.f, 0.f, 0.f);
    Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT_COMBINED PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_COMBINED Out;
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (0.0f == vDiffuse.a)
        discard;
    
    vector vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vSSAO = g_SSAOTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // vDiffuse * vShade + vSpecular;
    Out.vBackBuffer = ((vShade + vSpecular) * vSSAO);
    Out.vBloomScene = vShade;
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fFar;
    int iIdx = 0;

    [unroll]
    for (uint i = 0; i < 5; ++i)
        iIdx += (fViewZ >= g_fCascadeEnds[i + 1]);
    
    //캐스케이드가 5개니까..
    iIdx = min(iIdx, 4);
    
    vector vPosition;
    
    // 로컬위치 * 월드 * 뷰 * 투영 / w 
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
   
    
    // 로컬위치 * 월드 * 뷰 * 투영 
    // w 나누기 상쇄.
    vPosition = vPosition * fViewZ;
    
    // 로컬위치 * 월드 * 뷰  
    vPosition = mul(vPosition, g_ProjMatrixInv);
    
    // 로컬위치 * 월드   
    vPosition = mul(vPosition, g_ViewMatrixInv);
    
    vector vCascadePos, vStaticPos;
    
    vCascadePos = mul(vPosition, g_LightViewMatrix[iIdx]);
    vCascadePos = mul(vCascadePos, g_LightProjMatrix[iIdx]);
    
    vStaticPos = mul(vPosition, g_StaticLightViewMatrix);
    vStaticPos = mul(vStaticPos, g_StaticLightProjMatrix);
    
    //그림자 연산
    float fCSMFactor = Calc_Shadow_CSM(g_CascadeShadowTexture, vCascadePos, iIdx);
    float fShadowFactor = Calc_Shadow(g_StaticShadowTexture, vStaticPos);
    
    float fFinalFactor = min(fCSMFactor, fShadowFactor);
    
    Out.fShadow = fFinalFactor;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    Out.vBackBuffer = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);
    ////이미시브 샘플링.
    Out.vBackBuffer += Calc_Blur(g_EmissiveFinalTexture, In.vTexcoord);

    //안개 합성.
    Out.vBackBuffer = Calc_Fog(Out.vBackBuffer, g_FogTexture, g_vFogColor, In.vTexcoord);
    Out.vBackBuffer = Calc_VolumeFog(Out.vBackBuffer, g_VolumeFogTexture, In.vTexcoord);
    
    float4 vBlack = g_BlackBlendTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vBlur = saturate(Calc_Blur(g_BlurFinalTexture, In.vTexcoord));
    float4 vGlow = saturate(Calc_Glow(g_GlowFinalTexture, In.vTexcoord));
    float4 vGlowBloom = saturate(Calc_Glow(g_GlowBloomTexture, In.vTexcoord));
    float4 vMetaball = saturate(Calc_Glow(g_MetaballTexture, In.vTexcoord));
    
    Out.vBackBuffer.rgb = Out.vBackBuffer.rgb * (1 - vBlack.a) + vBlack.rgb * vBlack.a;
    Out.vBackBuffer.rgb = Out.vBackBuffer.rgb * (1 - vMetaball.a) + vMetaball.rgb * vMetaball.a;
    Out.vBackBuffer.rgb = Out.vBackBuffer.rgb * (1 - vGlow.a) + vGlow.rgb * vGlow.a;
    Out.vBackBuffer.rgb += vGlowBloom.rgb * vGlowBloom.a;
    Out.vBackBuffer.rgb = Out.vBackBuffer.rgb * (1 - vBlur.a) + vBlur.rgb * vBlur.a;
    
    Out.vBackBuffer += g_BloomTexture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_TONE_MAPPING(PS_IN In)
{
    PS_OUT_BACKBUFFER Out; 
    //Out.vBackBuffer = g_ScreenTexture.Sample(DefaultSampler, In.vTexcoord); 
    //Out.vBack Buffer.rgb = Out.vBackBuffer.rgb / (Out.vBackBuffer.rgb + 1); 
    //Out.vBackBuffer.a = 1.f; 
    
    Out.vBackBuffer = g_ScreenTexture.Sample(DefaultSampler, In.vTexcoord); 
    Out.vBackBuffer.rgb = pow(Out.vBackBuffer.rgb, 2.2f); 
    Out.vBackBuffer *= g_fHDRExposure;
    
    float a = 2.51f; 
    float b = 0.03f; 
    float c = 2.43f; 
    float d = 0.59f; 
    float e = 0.14f; 
    
    Out.vBackBuffer.rgb = saturate((Out.vBackBuffer.rgb * (a * Out.vBackBuffer.rgb + b)) / (Out.vBackBuffer.rgb * (c * Out.vBackBuffer.rgb + d) + e)); 
    Out.vBackBuffer.rgb = pow(Out.vBackBuffer.rgb, 1.0f / 2.2f); 
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
 
    Out.vBackBuffer = g_ScreenTexture.Sample(DefaultSampler, In.vTexcoord);

    return Out;
}

PS_OUT_SHADOW_BLUR_X PS_MAIN_SHADOW_BLUR_X(PS_IN In)
{
    PS_OUT_SHADOW_BLUR_X Out;
    
    float2 vTexcoord;
    float  fFactor = 0;
    float fWeightSum = 0;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + i / (float)g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        fFactor += g_fWeights[i + 6] * g_ShadowAreaTexture.Sample(ClampSampler, vTexcoord).x;
        fWeightSum += g_fWeights[i + 6];
    }
    
    Out.vBackBuffer = fFactor / fWeightSum;
    
    return Out;
}

PS_OUT_COMBINED_SHADOW PS_MAIN_COMBINE_SHADOW(PS_IN In)
{
    PS_OUT_COMBINED_SHADOW Out;
    
    float2 vTexcoord;
    float  fShadowFactor = 0;
    float  fWeightSum = 0;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + i / (float)g_iWinSizeY;
        
        fShadowFactor += g_fWeights[i + 6] * g_ShadowBlurXTexture.Sample(ClampSampler, vTexcoord).x;
        fWeightSum += g_fWeights[i + 6];
    }
    
    fShadowFactor = fShadowFactor / fWeightSum;
    
    Out.vBackBuffer = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord) * fShadowFactor;
    Out.vBloomScene = g_BloomSceneTexture.Sample(DefaultSampler, In.vTexcoord) * fShadowFactor;
    
    return Out;
}

//FXAA용 메서드. HDR 이후에 처리된다.
PS_OUT_BACKBUFFER PS_MAIN_FXAA(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
   
    Out.vBackBuffer = g_ScreenTexture.Sample(DefaultSampler, In.vTexcoord);

    float2 vTexel = float2(1.f / g_iWinSizeX, 1.f / g_iWinSizeY);
    float3 vColorN = g_ScreenTexture.Sample(DefaultSampler, In.vTexcoord + float2(0.f, vTexel.y * -1.f));
    float3 vColorS = g_ScreenTexture.Sample(DefaultSampler, In.vTexcoord + float2(0.f, vTexel.y * 1.f));
    float3 vColorW = g_ScreenTexture.Sample(DefaultSampler, In.vTexcoord + float2(vTexel.x * -1.f, 0.f));
    float3 vColorE = g_ScreenTexture.Sample(DefaultSampler, In.vTexcoord + float2(vTexel.x * 1.f, 0.f));
    float3 vColorM = g_ScreenTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float fLuminanceN = FxaaLuma(vColorN);
    float fLuminanceS = FxaaLuma(vColorS);
    float fLuminanceW = FxaaLuma(vColorW);
    float fLuminanceE = FxaaLuma(vColorE);
    float fLuminanceM = FxaaLuma(vColorM);
    
    //가장 휘도가 적은 곳.
    float fRangeMin = min(fLuminanceM, min(min(fLuminanceN, fLuminanceS), min(fLuminanceW, fLuminanceE)));
    //가장 휘도가 높은 곳. 
    float fRangeMax = max(fLuminanceM, max(max(fLuminanceN, fLuminanceS), max(fLuminanceW, fLuminanceE)));
    
    //contrast를 구한다.
    float fRange = fRangeMax - fRangeMin;
    
    if (fRange < max(FXAA_EDGE_THRESHOLD_MIN, fRangeMax * FXAA_EDGE_THRESHOLD)) 
        return Out;
    
    //휘도 차이가 많이 나는 곳으로 방향이 정해진다. 
    float2 vDir;
    vDir.x = -((fLuminanceN + fLuminanceS) - (fLuminanceE + fLuminanceW));
    vDir.y = ((fLuminanceN + fLuminanceS) - (fLuminanceE + fLuminanceW));
    
    vDir *= vTexel; // 텍셀 단위로 스케일

    float SPAN_MAX = 8.0f;
    vDir = clamp(vDir, -SPAN_MAX, SPAN_MAX) * vTexel;
    
    float3 vC1 = g_ScreenTexture.Sample(DefaultSampler, In.vTexcoord + vDir * -0.5).rgb;
    float3 vC2 = g_ScreenTexture.Sample(DefaultSampler, In.vTexcoord + vDir * -0.5).rgb;

    float3 vCA = 0.5 * (vC1 + vC2);
    float fLuminanceA = FxaaLuma(vCA);

    float3 vCB = vCA * 0.5 + Out.vBackBuffer.xyz * 0.5;
    float fLuminanceB = FxaaLuma(vCB);
    
    if (fLuminanceB < fRangeMin || fLuminanceB > fRangeMax)
        Out.vBackBuffer.xyz = vCA;
    else
        Out.vBackBuffer.xyz = vCB;

    // subpixel blend
    // 여기 인자화 해야됨.
    Out.vBackBuffer.xyz = lerp(Out.vBackBuffer.xyz, vCA, 0.6f);

    Out.vBackBuffer = float4(Out.vBackBuffer.xyz, 1.f);
    
    return Out;
}

technique11 DefaultTechnique
{ 
    // idx 0 
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }
    // idx 1
    pass Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }
    // idx 2
    pass Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }
    // idx 3
    pass VolumetricDirectional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_VOLUMETRIC_DIRECTIONAL();
    }
    // idx 4
    pass VolumetricPoint
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_VOLUMETRIC_POINT();
    }

    // idx 5
    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }
    // idx 6
    pass Deferred
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }

    // idx 7
    pass ToneMapping
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TONE_MAPPING();
    }

    // idx 8
    pass Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

    // idx 9
    pass Spot
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPOT();
    }

    // idx 10
    pass Occlusion
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = NULL;
    }
    // idx 11 
    pass ShadowBlurX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW_BLUR_X();
    }
    // idx 12
    pass Combine_Shadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINE_SHADOW();
    }
    // idx 13
    pass FXAA
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FXAA();
    }
}

/*
PS_OUT_LIGHT PS_MAIN_VOLUMETRIC_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vPosition;
    vector vShadowPosition;
    vector vDepth = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepth.x;
    vPosition.w = 1.f;
    float fViewZ = vDepth.y * 500.f;
    
    vPosition *= fViewZ;

    vPosition = mul(vPosition, g_ProjMatrixInv);
    vPosition = mul(vPosition, g_ViewMatrixInv);
    
    vector vRayFromCamera = normalize(vPosition - g_vCamPosition);
    
    float fEnd = length(vPosition.xyz - g_vCamPosition.xyz);
    fEnd = min(fEnd, 100.f);
    
    float fT = 0.f;
    float fTrans = 1.f;
    float3 vResult = float3(0.f, 0.f, 0.f);
    
    float g = g_fVolumetricG;
    float LdotV = dot(normalize(g_vLightDir.xyz) * -1.f, vRayFromCamera.xyz);
    float fPhase = (1.0f - g * g) / (pow(1.0f + g * g - 2.0f * g * LdotV, 1.5f));
    
    int iCount = 0;
    
    while (fT < fEnd && fTrans > 0.01f && iCount < 64)
    {
        float3 vSamplePos = g_vCamPosition.xyz + vRayFromCamera.xyz * fT;

        vector vShadowPosition = mul(vector(vSamplePos, 1.f), g_LightViewMatrix);
        vShadowPosition = mul(vShadowPosition, g_LightProjMatrix);
        
        float2 vShadowUV;
        vShadowUV.x = ((vShadowPosition.x / vShadowPosition.w) * 0.5f + 0.5f);
        vShadowUV.y = ((vShadowPosition.y / vShadowPosition.w) * -0.5f + 0.5f);

        float fShadowMapDepth = g_ShadowTexture.Sample(DefaultSampler, vShadowUV).r * 500.0f;
        float fCurrentDepth = vShadowPosition.w;

        float fBias = 0.05f;
        float fShadow = (fCurrentDepth - fBias > fShadowMapDepth) ? 1.0f : 0.0f;
        float fVisibility = 1.0f - fShadow;

        fTrans *= exp(-g_fDensity * g_fStepSize);
        
        float3 vInScatter = g_vLightDiffuse * g_fDensity * fPhase * fVisibility;

        vResult += vInScatter * fTrans * g_fStepSize;
        fT += g_fStepSize;
        iCount++;
    }
    
    Out.vShade = float4(vResult, 1.f);
    Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}
*/