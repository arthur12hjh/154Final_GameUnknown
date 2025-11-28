#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_GlowTexture;
texture2D g_WeightTexture;
texture2D g_WeightFinalTexture;


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

PS_OUT_GLOW_X PS_MAIN_GLOW_X(PS_IN In)
{
    PS_OUT_GLOW_X Out;
    
    float2 vTexcoord;
    float4 vColor;
    float4 vWeight;
    float vsize;
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 6] * g_GlowTexture.Sample(ClampSampler, vTexcoord);
        vWeight += g_fWeights[i + 6] * g_WeightTexture.Sample(ClampSampler, vTexcoord);
        vsize += g_fWeights[i + 6];

    }
    
    Out.vGlowX = vColor / vsize;
    Out.vGlowX.rgb *= 5;
    
    Out.vWeight = vWeight / vsize;
    return Out;
}

PS_OUT_GLOW_FINAL PS_MAIN_GLOW_FINAL(PS_IN In)
{
    PS_OUT_GLOW_FINAL Out;
    float2 vTexcoord;
    float4 vColor;
    float4 vWeight;
    float vsize;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_iWinSizeY;
        
        vColor += g_fWeights[i + 6] * g_GlowTexture.Sample(ClampSampler, vTexcoord);
        vWeight += g_fWeights[i + 6] * g_WeightTexture.Sample(ClampSampler, vTexcoord);
        vsize += g_fWeights[i + 6];
    }
    vColor /= vsize;
    
    Out.vGlowY = vColor;
    Out.vWeight = vWeight / vsize;
    return Out;
}

PS_OUT_GLOW_FINAL PS_MAIN_GLOW_REAL_FINAL(PS_IN In)
{
    PS_OUT_GLOW_FINAL Out;
    Out.vGlowY = g_GlowTexture.Sample(ClampSampler, In.vTexcoord * 6);
    Out.vWeight = g_WeightTexture.Sample(ClampSampler, In.vTexcoord);
    return Out;
}

PS_OUT_GLOW_X PS_MAIN_CLEAN_GLOW_X(PS_IN In)
{
    PS_OUT_GLOW_X Out;
    
    float2 vTexcoord;
    float4 vColor;
    float4 vWeight;
    float vsize;
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 6] * g_GlowTexture.Sample(ClampSampler, vTexcoord);
        vWeight += g_fWeights[i + 6] * g_WeightTexture.Sample(ClampSampler, vTexcoord);
        vsize += g_fWeights[i + 6];

    }
    
    Out.vGlowX = vColor / vsize;
    Out.vGlowX.rgb *= 5;
    
    Out.vWeight = vWeight / vsize;
    return Out;
}

PS_OUT_GLOW_FINAL PS_MAIN_CLEAN_GLOW_FINAL(PS_IN In)
{
    PS_OUT_GLOW_FINAL Out;
    float2 vTexcoord;
    float4 vColor;
    float4 vWeight;
    float vsize;
    if (0 >= g_WeightFinalTexture.Sample(ClampSampler, In.vTexcoord).g)
        discard;
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_iWinSizeY;
    
        vColor += g_fWeights[i + 6] * g_GlowTexture.Sample(ClampSampler, vTexcoord);
        vWeight += g_fWeights[i + 6] * g_WeightTexture.Sample(ClampSampler, vTexcoord);
        vsize += g_fWeights[i + 6];
    }
    vColor /= vsize;
    
    Out.vGlowY = vColor;
    Out.vWeight = vWeight / vsize;
    return Out;
}

PS_OUT_GLOW_FINAL PS_MAIN_CLEAN(PS_IN In)
{
    PS_OUT_GLOW_FINAL Out;
    Out.vGlowY = g_GlowTexture.Sample(ClampSampler, In.vTexcoord);
    Out.vWeight = g_WeightTexture.Sample(ClampSampler, In.vTexcoord);
    return Out;
}

technique11 DefaultTechnique
{
    // idx 0
    pass Glow_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLOW_X();
    }
    // idx 1
    pass Glow_Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLOW_FINAL();
    }
    // idx 2
    pass Glow_Real_Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLOW_REAL_FINAL();
    }
    // idx 3
    pass Clean_Glow_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CLEAN_GLOW_X();
    }
    // idx 4
    pass Clean_Glow_Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CLEAN_GLOW_FINAL();
    }
    // idx 5
    pass Clean
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CLEAN();
    }
}