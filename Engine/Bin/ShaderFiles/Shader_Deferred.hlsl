#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;

//안개 색은 렌더타겟에 세팅해놓는다.
float4 g_vFogColor;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;
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
texture2D g_SSAOTexture;
texture2D g_SSSAOTexture;

texture2D g_BlurFinalTexture;
texture2D g_GlowFinalTexture;
texture2D g_BlurWeightTexture;
texture2D g_GlowWeightTexture;
texture2D g_EmissiveFinalTexture;

texture2D g_DistortionTexture;
texture2D g_FogTexture;
texture2D g_BloomTexture;
texture2D g_VolumetricTexture;

texture2D g_SceneTexture;
texture2D g_ScreenTexture;

vector g_vLightDir;
vector g_vLightPos;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;
float g_fLightRange;
float g_fFalloff;
float g_fTheta;
float g_fPhi;

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

//---------------------------------------------------
// MAIN
//---------------------------------------------------
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;

    float4 vORMDesc = g_ORMTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vAlbedo = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    // Normal Strength 증가 --------------------------------------------
    float3 N;
    {
        float3 n = vNormalDesc.xyz * 2.f - 1.f;
        float normalStrength = 0.8f; // 기존 0.35 -> 최소 수정
        n.xy *= normalStrength;
        N = normalize(n);
    }

    // 위치 복원 그대로 ---------------------------------------------------
    float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float viewZ = vDepthDesc.y * 500.f;

    float4 pos;
    pos.x = In.vTexcoord.x * 2.f - 1.f;
    pos.y = In.vTexcoord.y * -2.f + 1.f;
    pos.z = vDepthDesc.x;
    pos.w = 1.f;

    pos *= viewZ;
    pos = mul(pos, g_ProjMatrixInv);
    pos = mul(pos, g_ViewMatrixInv);

    float3 V = normalize((pos - g_vCamPosition).xyz);
    float3 L = normalize(g_vLightDir.xyz) * -1.f;

    float AO = vORMDesc.r;
    float Rough = vORMDesc.g;
    float MetalSrc = vORMDesc.b;
    float A = vORMDesc.a;

    float Metallic = 0.f;
    float3 F0 = float3(0.04f, 0.04f, 0.04f);

    bool hasPacked = (AO != 0.f || Rough != 0.f || MetalSrc != 0.f || A != 0.f);
    bool isORSS = hasPacked && (A > 0.f);
    bool isORM = hasPacked && (A == 0.f);

    // Fallback ---------------------------------------------------------
    if (!hasPacked)
    {
        float NdotL = saturate(dot(N, L));

        float3 diff = vAlbedo.rgb * g_vLightDiffuse.rgb *
                      saturate(NdotL + (g_vLightAmbient.rgb * g_vMtrlAmbient.rgb));

        float3 refl = reflect(-L, N);
        float3 spec = (g_vLightSpecular.rgb * g_vMtrlSpecular.rgb) *
                      pow(max(dot(-V, normalize(refl)), 0.f), 50.f);

        Out.vShade = float4(diff, 1);
        Out.vSpecular = float4(spec, 1);
        return Out;
    }

    // ORSS(피부)
    if (isORSS)
    {
        AO = vORMDesc.r;
        Rough = max(vORMDesc.g, 0.05f);
        float specFactor = saturate(vORMDesc.b);
        Metallic = 0.f;

        float3 baseF0 = float3(0.028f, 0.028f, 0.028f);
        F0 = baseF0 + specFactor * 0.05f;
    }
    else if (isORM)
    {
        AO = vORMDesc.r;
        Rough = max(vORMDesc.g, 0.05f);
        Metallic = saturate(vORMDesc.b);
        F0 = lerp(float3(0.04f, 0.04f, 0.04f), vAlbedo.rgb, Metallic);
    }

    // AO 증폭 완화 ------------------------------------------------------
    float AOStr = lerp(1.0f, 1.2f, AO); // 기존 1.3~2.0 → 최소 수정
    Out = PBR_Light(N, -V, L, vAlbedo.rgb, Metallic, Rough, g_vLightDiffuse.xyz, 1.f, F0, 1.f);
    Out.vShade.rgb *= AOStr;

    //-------------------------------------------------------
    // SSS Back-scattering 추가 (역광 문제 해결)
    //-------------------------------------------------------
    if (isORSS)
    {
        float3 base = Out.vShade.rgb;

        float luma = dot(base, float3(0.299f, 0.587f, 0.114f));
        float3 bloodHue = float3(1.0f, 0.45f, 0.45f);
        float3 bloodColor = bloodHue * luma;

        float ndl = saturate(dot(N, L));
        float edge = pow(1.0f - ndl, 1.2f);
        float sss = saturate(A * g_SSSAOTexture.Sample(DefaultSampler, In.vTexcoord).g);
        float w = saturate(sss * edge * 0.35f);

        float3 result = lerp(base, bloodColor, w);
        Out.vShade.rgb = result;

        // 역광 SSS BackScatter ----------------------------------------
        float back = saturate(dot(-N, L));
        float3 backSSS = vAlbedo.rgb * float3(1.0f, 0.3f, 0.25f)
                         * pow(back, 1.1f) * 0.35f;

        Out.vShade.rgb += backSSS;
    }

    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vORMDesc = g_ORMTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 500.f;
    
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
    
    vector vLightDir = vPosition - g_vLightPos;
    
    float fDistance = length(vLightDir);
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    
    if (fAtt <= 0.f)
        discard;
    
    vector vLook = vPosition - g_vCamPosition;
    vector vReflect = reflect(normalize(vLightDir), vNormal);
    vector vAlbedo = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float fMetallic, fRoughness, fOcclusion, fAttenuation;
    float3 vF0;
    
    //ORM 마스크 없으면 그냥 Phong Shading 처리.
    if (vORMDesc.r == 0 && vORMDesc.g == 0 && vORMDesc.b == 0 && vORMDesc.a == 0)
    {
        Out.vShade = fAtt * (g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)));
        Out.vSpecular = fAtt * ((g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f));
        
        return Out;
    }
    else if (vORMDesc.a == 2.f)
    {
        fMetallic = 0.f;
        fRoughness = vORMDesc.g;
        fOcclusion = vORMDesc.b;
        vF0 = float3(0.04f, 0.04f, 0.04f);
    }
    else
    {
        fOcclusion = vORMDesc.r;
        fRoughness = vORMDesc.g;
        fMetallic = vORMDesc.b;
        vF0 = lerp(float3(0.04f, 0.04f, 0.04f), vAlbedo.xyz, fMetallic);
    }
    
    fAttenuation = 1.f;
    
    float fDiffuseAOStrength = lerp(1.3f, 2.0f, fOcclusion);
    
    Out = PBR_Light(normalize(vNormal.xyz), normalize(vLook.xyz) * -1.f, normalize(g_vLightDir.xyz) * -1.f, vAlbedo.xyz, fMetallic, fRoughness, g_vLightDiffuse.xyz, fAttenuation, vF0, 1.f);
    
    Out.vShade *= fDiffuseAOStrength * fAtt;
    Out.vSpecular *= fAtt;
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 500.f;
    
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
    
    //
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
    
    vector vSurfaceNormal = normalize(vNormal);
    vector vViewDir = normalize(vPosition - g_vCamPosition) * -1.f;
    vector vReflect = reflect(vLightDir, vSurfaceNormal);
    
    //vector vLook = vPosition - g_vCamPosition;
    //vector vReflect = reflect(normalize(vLightDir), vNormal);
    
    Out.vShade = fFinalAtt * (g_vLightDiffuse * saturate(max(dot(vSurfaceNormal, vLightDir), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)));
    Out.vSpecular = fFinalAtt * ((g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(vViewDir, vReflect), 0.f), 50.f));
    
    return Out;
}

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
    vector vVolumetric = g_VolumetricTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vSSAO = g_SSAOTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // vDiffuse * vShade + vSpecular;
    Out.vBackBuffer = ((vShade + vSpecular) * vSSAO) + vVolumetric;
    Out.vBloomScene = vShade;
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 500.f;
    
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
    
    vPosition = mul(vPosition, g_LightViewMatrix);
    vPosition = mul(vPosition, g_LightProjMatrix);
    
    //그림자 연산
    Out.vBackBuffer = Calc_Shadow(Out.vBackBuffer, g_ShadowTexture, vPosition);
    Out.vBloomScene = Calc_Shadow(Out.vBloomScene, g_ShadowTexture, vPosition);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    Out.vBackBuffer = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);
    ////이미시브 샘플링.
    Out.vBackBuffer += Calc_Blur(g_EmissiveFinalTexture, In.vTexcoord);

    //블룸 샘플링
    Out.vBackBuffer += g_BloomTexture.Sample(DefaultSampler, In.vTexcoord);
    //디스토션 샘플링.
    Out.vBackBuffer = Calc_Distortion(Out.vBackBuffer, g_SceneTexture, g_DistortionTexture, In.vTexcoord);
    //안개 합성.
    Out.vBackBuffer = Calc_Fog(Out.vBackBuffer, g_FogTexture, g_vFogColor, In.vTexcoord);
    
    
    vector fBlurColor = Calc_Blur(g_BlurFinalTexture, In.vTexcoord) + Calc_Glow(g_GlowFinalTexture, In.vTexcoord);
    vector fBlurAlpha = Calc_Blur(g_BlurWeightTexture, In.vTexcoord) + Calc_Glow(g_GlowWeightTexture, In.vTexcoord);
    //fBlurAlpha.r = saturate(Calc_Blur(g_BlurWeightTexture, In.vTexcoord).r + Calc_Glow(g_GlowWeightTexture, In.vTexcoord).r);
    //fBlurAlpha.g = saturate(Calc_Blur(g_BlurWeightTexture, In.vTexcoord).g + Calc_Glow(g_GlowWeightTexture, In.vTexcoord).g);
    vector fBlur;
    fBlur.rgb = fBlurColor.rgb / (fBlurAlpha.r);
    fBlur.a = saturate(fBlurAlpha.r);
    
    Out.vBackBuffer.rgb = Out.vBackBuffer.rgb * (1 - fBlur.a) + saturate(fBlur.rgb) * fBlur.a;
    
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