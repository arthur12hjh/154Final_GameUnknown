#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;
float g_fFar;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_SceneTexture;
texture2D g_SourTexture;
texture2D g_ScreenTexture;
texture2D g_BloomTexture;

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
};

PS_OUT_BACKBUFFER PS_MAIN(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    Out.vBackBuffer = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
};

PS_OUT_BACKBUFFER PS_MAIN_CURVE(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
  
    float3 vColor = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord).xyz;
    
    //0 나누기 방지..
    float fIntensity = dot(vColor, float3(0.3f, 0.3f, 0.3f)) + 0.00001f;
    float fBloomIntensity = GetBloomCurve(dot(vColor, float3(0.3f, 0.3f, 0.3f)));

    float3 vBloomColor = vColor * fBloomIntensity / fIntensity;
    
    Out.vBackBuffer = float4(vBloomColor, 1.f);
    
    return Out;
};

PS_OUT_BLUR PS_MAIN_SAMPLING_BLUR_X(PS_IN In)
{
    PS_OUT_BLUR Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    
    for (int i = -10; i < 11; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fBloomWeights[i + 10] * g_SceneTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBlur = vColor / 10.f;
    
    return Out;
}

PS_OUT_BLUR PS_MAIN_SAMPLING_BLUR_Y(PS_IN In)
{
    PS_OUT_BLUR Out;
    
    float2 vTexcoord;
    float4 vColor = 0;
    
    for (int i = -10; i < 11; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_iWinSizeY;
        
        vColor += g_fBloomWeights[i + 10] * g_SceneTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBlur = vColor / 6.5f;
    
    return Out;
}

PS_OUT_BLUR PS_MAIN_ADDITIVE_BLUR_X(PS_IN In)
{
    PS_OUT_BLUR Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    
    for (int i = -10; i < 11; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fBloomWeights[i + 10] * (g_SceneTexture.Sample(ClampSampler, vTexcoord) + g_SourTexture.Sample(ClampSampler, vTexcoord));
    }
    
    Out.vBlur = vColor / 10.f;
    
    return Out;
}

PS_OUT_BLUR PS_MAIN_ADDITIVE_BLUR_Y(PS_IN In)
{
    PS_OUT_BLUR Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    
    for (int i = -10; i < 11; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_iWinSizeY;
        
        vColor += g_fBloomWeights[i + 10] * g_SceneTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBlur = vColor / 6.5f;
    
    return Out;
}

technique11 DefaultTechnique
{ 
    // 일반 샘플링
    pass Sampling
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    // 커브
    pass Curve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CURVE();
    }

    pass Sampling_BlurX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SAMPLING_BLUR_X();
    }

    pass Sampling_Blur_Y
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SAMPLING_BLUR_Y();
    }

    pass Additive_Blur_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ADDITIVE_BLUR_X();
    }
    pass Additive_Blur_Y
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ADDITIVE_BLUR_Y();
    }
}