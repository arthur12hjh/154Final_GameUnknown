#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;
texture2D g_Texture;
vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;
vector g_vCamPosition;

texture2D g_RimLightTexture;
texture2D g_NormalTexture;
texture2D g_DiffuseTexture;
texture2D g_ShadeTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;
texture2D g_ShadowTexture;

texture2D g_BlurFinalTexture;
texture2D g_GlowFinalTexture;
texture2D g_BlurWeightTexture;
texture2D g_GlowWeightTexture;

texture2D g_DistortionTexture;
texture2D g_BloomTexture;

texture2D g_SceneTexture;
texture2D g_ScreenTexture;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

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
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    
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
    
    vector vLook = vPosition - g_vCamPosition;
    vector vReflect = reflect(normalize(g_vLightDir), vNormal);
    
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
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
    
    vector vLightDir = vPosition - g_vLightPos;
    float fDistance = length(vLightDir);
    
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    
    Out.vShade = fAtt * (g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)));
    
    vector vLook = vPosition - g_vCamPosition;
    vector vReflect = reflect(normalize(vLightDir), vNormal);
    
    Out.vSpecular = fAtt * ((g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f));
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (0.0f == vDiffuse.a)
        discard;
    vector vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vRimLight = g_RimLightTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vBackBuffer = vDiffuse * vShade + vSpecular + vRimLight;
    //vSpecular + vRimLight;
    
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
    
    vPosition = mul(vPosition, g_LightViewMatrix);
    vPosition = mul(vPosition, g_LightProjMatrix);
    
    /* -1, 1 -> 0, 0  */
    /* 1, -1 -> 1, 1  */
    
    //그림자 연산
    Out.vBackBuffer = Calc_Shadow(Out.vBackBuffer, g_ShadowTexture, vPosition);
   
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    Out.vBackBuffer = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);
    ////블러 샘플링.
    //Out.vBackBuffer += Calc_Blur(g_BlurFinalTexture, In.vTexcoord);
    ////글로우 샘플링.
    //Out.vBackBuffer += Calc_Glow(g_GlowFinalTexture, In.vTexcoord);
    
    //블룸 샘플링
    Out.vBackBuffer += g_BloomTexture.Sample(DefaultSampler, In.vTexcoord);
    //디스토션 샘플링.
    Out.vBackBuffer = Calc_Distortion(Out.vBackBuffer, g_SceneTexture, g_DistortionTexture, In.vTexcoord);
    
    
    vector fBlurColor = Calc_Blur(g_BlurFinalTexture, In.vTexcoord) + Calc_Glow(g_GlowFinalTexture, In.vTexcoord);
    vector fBlurAlpha = Calc_Blur(g_BlurWeightTexture, In.vTexcoord) + Calc_Glow(g_GlowWeightTexture, In.vTexcoord);
    
    vector fBlur;
    fBlur.rgb = fBlurColor.rgb / fBlurAlpha.r;
    fBlur.a = saturate(fBlurAlpha.r);
    
    Out.vBackBuffer.rgb = Out.vBackBuffer.rgb * (1 - fBlur.a) + saturate(fBlur.rgb) * fBlur.a;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_SCREEN_RADIAL_BLUR(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
 
    float4 vAccumulatedColor = 0.0f;
    float iSampleCount = 3;

    float2 vDir = float2(0.5f, 0.5f) - In.vTexcoord;
    vDir *= length(float2(0.5f, 0.5f) - In.vTexcoord) * 0.5f;
    
    for (int i = 0; i < iSampleCount; i++)
    {
        // 샘플링 진행률 (0.0 ~ 1.0)
        float t = (float) i / (float) iSampleCount;
        
        // 샘플링 UV 좌표: 현재 UV + (방향 벡터 * 진행률)
        float2 vSampleUV = In.vTexcoord + vDir * t;
        
        // 텍스처 샘플링 및 누적
        vAccumulatedColor += g_ScreenTexture.Sample(DefaultSampler, vSampleUV);
    }
    
    // 누적된 색상을 샘플 개수로 나누어 평균을 구함
    Out.vBackBuffer = vAccumulatedColor / (float) iSampleCount;

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
    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }
    // idx 4
    pass Deferred
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }

    // idx 5
    pass Screen_RadialBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SCREEN_RADIAL_BLUR();
    }
    // idx 6
    pass Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }
}