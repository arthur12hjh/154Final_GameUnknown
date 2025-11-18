#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;

//안개 색은 렌더타겟에 세팅해놓는다.
float4 g_vFogColor;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;
texture2D g_Texture;
vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;
vector g_vCamPosition;

texture2D g_NormalTexture;
texture2D g_DiffuseTexture;
texture2D g_ShadeTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;
texture2D g_ORMTexture;
texture2D g_ShadowTexture;

texture2D g_BlurFinalTexture;
texture2D g_GlowFinalTexture;
texture2D g_BlurWeightTexture;
texture2D g_GlowWeightTexture;

texture2D g_DistortionTexture;
texture2D g_FogTexture;
texture2D g_BloomTexture;

texture2D g_SceneTexture;
texture2D g_ScreenTexture;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

bool            g_bDebugTextureArray;
int             g_CasCadeIndex;
Texture2DArray  g_CasCadeMap : register(t5);
cbuffer g_ConstantCasCadeBuffer : register(b0)
{
    matrix ShadowWorld;
    matrix ShadowVPMatrix[3];
    float CasCadeDist[3];
    float Padding; // <- 이거 무의미한 값이긴한데 나중에 패딩 제거해주겠음
};

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
    
    if(g_bDebugTextureArray)
    {
        float vData = g_CasCadeMap.Sample(DefaultSampler, float3(In.vTexcoord, 0)).r;
        float vData1 = g_CasCadeMap.Sample(DefaultSampler, float3(In.vTexcoord, 1)).r;
        float vData2 = g_CasCadeMap.Sample(DefaultSampler, float3(In.vTexcoord, 2)).r;
        Out.vBackBuffer = float4(vData, vData1, vData2, 1.f);
    }
    else
        Out.vBackBuffer = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;   
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vORMDesc = g_ORMTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f));
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
    
    vector N = normalize(vNormal);
    vector L = normalize(g_vLightDir) * -1.f;
    vector V = normalize(vLook) * -1.f;
    vector H = normalize(L + V);

    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vColor = LightSurface(V.xyz, N.xyz, g_vLightDiffuse.xyz, normalize(g_vLightDir.xyz), vDiffuse.rgb, vORMDesc.g, vORMDesc.b, vORMDesc.a);
    
    //ORM 마스크 없으면 그냥 Phong Shading 처리 해.
    if (vORMDesc.r == 0 && vORMDesc.g == 0 && vORMDesc.b == 0 && vORMDesc.a == 0)
    {
        Out.vShade = vDiffuse * g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f);
    }
    //Shade 처리해서 
    else
    {
        Out.vShade = float4(vColor, 1.f);
        Out.vSpecular = 0.f;
    }
    
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
    
    float  fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    vector vLook = vPosition - g_vCamPosition;
    vector vReflect = reflect(normalize(vLightDir), vNormal);
    
    Out.vShade = fAtt * (g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)));
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
    
    Out.vBackBuffer = vShade + vSpecular; //vDiffuse * vShade + vSpecular;

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
    
    float fDist = length(g_vCamPosition - vPosition);

    //그림자 연산
    Out.vBackBuffer = Calc_Shadow(Out.vBackBuffer, g_ShadowTexture, vPosition);
    //float3 vShadow = Calc_Shadow3x3(g_CasCadeMap, ShadowSampler, CasCadeDist, ShadowVPMatrix, vPosition, fDist);
    Out.vBackBuffer.xyz *= vShadow;
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
    //안개 합성.
    Out.vBackBuffer = Calc_Fog(Out.vBackBuffer, g_FogTexture, g_vFogColor, In.vTexcoord);
    
    
    vector fBlurColor = Calc_Blur(g_BlurFinalTexture, In.vTexcoord) + Calc_Glow(g_GlowFinalTexture, In.vTexcoord);
    vector fBlurAlpha = Calc_Blur(g_BlurWeightTexture, In.vTexcoord) + Calc_Glow(g_GlowWeightTexture, In.vTexcoord);
    
    vector fBlur;
    fBlur.rgb = fBlurColor.rgb / (fBlurAlpha.r * fBlurAlpha.g);
    fBlur.a = saturate(fBlurAlpha.r * fBlurAlpha.g);
    
    Out.vBackBuffer.rgb = Out.vBackBuffer.rgb * (1 - fBlur.a) + saturate(fBlur.rgb) * fBlur.a;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_SCREEN_RADIAL_BLUR(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
 
    float4 vAccumulatedColor = 0.0f;
    int iSampleCount = 3;
    float fSamplePower = 0.5f;
    
    float2 vDir = float2(0.5f, 0.5f) - In.vTexcoord;
    vDir *= length(float2(0.5f, 0.5f) - In.vTexcoord) * fSamplePower;
    
    for (int i = 0; i < iSampleCount; i++)
    {
        float fSampleRate = (float) i / (float) iSampleCount;
        
        // 샘플링 UV 좌표: 현재 UV + (방향 벡터 * 진행률)
        float2 vSampleUV = In.vTexcoord + vDir * fSampleRate;
        
        // 텍스처 샘플링 및 누적
        vAccumulatedColor += g_ScreenTexture.Sample(DefaultSampler, vSampleUV);
    }
    
    Out.vBackBuffer = vAccumulatedColor / (float) iSampleCount;

    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_TONE_MAPPING(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
 
    //Out.vBackBuffer = g_ScreenTexture.Sample(DefaultSampler, In.vTexcoord);
    
    //Out.vBackBuffer.rgb = Out.vBackBuffer.rgb / (Out.vBackBuffer.rgb + 1);

    //Out.vBackBuffer.a = 1.f;
    Out.vBackBuffer = g_ScreenTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vBackBuffer.rgb = pow(Out.vBackBuffer.rgb, 2.2f);
    
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
    pass ToneMapping
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TONE_MAPPING();
    }

    // idx 7
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

/*
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vORMDesc = g_ORMTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f));
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 500.f;
    vector vPosition;


    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

    vPosition = vPosition * fViewZ;

    vPosition = mul(vPosition, g_ProjMatrixInv);

    vPosition = mul(vPosition, g_ViewMatrixInv);
    
    vector vLook = vPosition - g_vCamPosition;
    vector vReflect = reflect(normalize(g_vLightDir), vNormal);
    
    vector N = normalize(vNormal);
    vector L = normalize(g_vLightDir) * -1.f;
    vector V = normalize(vLook) * -1.f;
    vector H = normalize(L + V);
    
    float fNdotL = max(dot(N, L), 0.f);
    float fNdotH = max(dot(N, H), 0.f);
    float fNdotV = max(dot(N, V), 0.f);
    float fVdotH = max(dot(V, H), 0.f);
    float fLdotH = max(dot(L, H), 0.f);
    
    Out.vShade = g_vLightDiffuse * saturate(max(fNdotL, 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    Out.vSpecular = Specular_BRDF(pow(vORMDesc.g, 2), g_vLightSpecular, fNdotH, fNdotV, fNdotL, fVdotH) * fNdotL;

    return Out;
}
*/