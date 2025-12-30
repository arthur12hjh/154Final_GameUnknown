#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;
float g_fFar;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_BlurTexture;

texture2D g_EmissiveTexture;

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

PS_OUT_BLUR PS_BLUR_WEIGHT(PS_IN In)
{
    PS_OUT_BLUR Out;
    float4 BlurColor = g_BlurTexture.Sample(ClampSampler, In.vTexcoord);
    if (0 >= BlurColor.a)
        discard;
    Out.vBlur.rgb = BlurColor.rgb / BlurColor.a;
    Out.vBlur.a = saturate(BlurColor.a);
    return Out;
}

PS_OUT_BLUR PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT_BLUR Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    float vSize = 0.f;
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float)i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 6] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fWeights[i + 6];
    }
    
    Out.vBlur = vColor / vSize;
    return Out;    
}

PS_OUT_BLUR PS_MAIN_BLUR_FINAL(PS_IN In)
{
    PS_OUT_BLUR Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    float vSize = 0.f;
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_iWinSizeY;
        
        vColor += g_fWeights[i + 6] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fWeights[i + 6];

    }
    
    Out.vBlur = vColor / vSize;
    
    return Out;
}

PS_OUT_BLUR PS_MAIN_EMISSIVE_X(PS_IN In)
{
    PS_OUT_BLUR Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    
    float vSize = 0.f;
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 6] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fWeights[i + 6];
    }

    Out.vBlur = vColor / vSize * 2.f;
    
    //Out.vBlur = g_BlurTexture.Sample(ClampSampler, In.vTexcoord) * 2.f;
    return Out;
}

PS_OUT_BLUR PS_MAIN_EMISSIVE_Y(PS_IN In)
{
    PS_OUT_BLUR Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    float vSize = 0.f;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_iWinSizeY;
        
        vColor += g_fWeights[i + 6] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fWeights[i + 6];
    }
    
    Out.vBlur = vColor / vSize * 2.f;

    
    //Out.vBlur = g_BlurTexture.Sample(ClampSampler, In.vTexcoord) * 2.f;
    
    return Out;
}

PS_OUT_BLUR PS_MAIN_SMOK_X(PS_IN In)
{
    PS_OUT_BLUR Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    float vSize = 0.f;
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 6] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fWeights[i + 6];
    }
    
    Out.vBlur = vColor / vSize;
    return Out;
}

PS_OUT_BLUR PS_MAIN_SMOK_FINAL(PS_IN In)
{
    PS_OUT_BLUR Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    float vSize = 0.f;
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_iWinSizeY;
        
        vColor += g_fWeights[i + 6] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fWeights[i + 6];

    }
    Out.vBlur = vColor / vSize;
    
    return Out;
}

technique11 DefaultTechnique
{
    // idx 0
    pass Blur_Weight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_WEIGHT();
    }
    // idx 1
    pass Blur_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }
    // idx 2
    pass Blur_Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_FINAL();
    }

    // idx 3
    pass EMISSIVE_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE_X();
    }
    // idx 4
    pass EMISSIVE_FINAL
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE_Y();
    }
}