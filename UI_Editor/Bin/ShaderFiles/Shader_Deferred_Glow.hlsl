#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_GlowTexture;
texture2D g_WeightTexture;


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
    
    for (int i = -10; i < 11; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fBloomWeights[i + 10] * g_GlowTexture.Sample(ClampSampler, vTexcoord);
        vWeight += g_fBloomWeights[i + 10] * g_WeightTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vGlowX = vColor / 10.f;
    
    Out.vGlowX.rgb *= 5;
    Out.vGlowX.a *= 5;
    //Out.vGlowX *= 0.01;
    
    
    Out.vWeight = vWeight / 10.f;
    
    //Out.vWeight *= 0.01;
    return Out;
}

PS_OUT_GLOW_FINAL PS_MAIN_GLOW_FINAL(PS_IN In)
{
    PS_OUT_GLOW_FINAL Out;
    float2 vTexcoord;
    float4 vColor;
    float4 vWeight;
    
    for (int i = -10; i < 11; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fBloomWeights[i + 10] * g_GlowTexture.Sample(ClampSampler, vTexcoord);
        vWeight += g_fBloomWeights[i + 10] * g_WeightTexture.Sample(ClampSampler, vTexcoord);
    }
    vColor /= 6.5f;
    
    Out.vGlowY = vColor;
    Out.vWeight = vWeight / 6.5f;
    
    return Out;
}

PS_OUT_GLOW_FINAL PS_MAIN_GLOW_REAL_FINAL(PS_IN In)
{
    PS_OUT_GLOW_FINAL Out;
    Out.vGlowY = saturate(g_GlowTexture.Sample(ClampSampler, In.vTexcoord) * 6);
    Out.vWeight = max(max(Out.vGlowY.r, Out.vGlowY.g), Out.vGlowY.b) * g_WeightTexture.Sample(ClampSampler, In.vTexcoord).g;
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
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
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
        PixelShader = compile ps_5_0 PS_MAIN_GLOW_FINAL();
    }
}