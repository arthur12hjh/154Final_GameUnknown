#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_BlurTexture;
texture2D g_WeightTexture;

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

PS_OUT_BLUR_X PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT_BLUR_X Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    float4 vWeight = 0.f;
    float vSize;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float)i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 6] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
        vWeight += g_fWeights[i + 6] * g_WeightTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fWeights[i + 6];
    }
    
    Out.vBlurX = vColor / vSize;
    Out.vWeight = vWeight / vSize;
    
    return Out;    
}

PS_OUT_BLUR_FINAL PS_MAIN_BLUR_FINAL(PS_IN In)
{
    PS_OUT_BLUR_FINAL Out;
    
    float2 vTexcoord;
    float4 vColor;
    float4 vWeight = 0.f;
    float vSize;
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_iWinSizeY;
        
        vColor += g_fWeights[i + 6] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
        vWeight += g_fWeights[i + 6] * g_WeightTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fWeights[i + 6];

    }
    
    Out.vBlurY = vColor / vSize;
    Out.vWeight = (vWeight / vSize);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_EMISSIVE_BLUR_X(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    float vSize;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 6] * g_EmissiveTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fWeights[i + 6];
    }
    
    Out.vBackBuffer = vColor / vSize;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_EMISSIVE_BLUR_FINAL(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    float vSize;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_iWinSizeY;
        
        vColor += g_fWeights[i + 6] * g_EmissiveTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fWeights[i + 6];
    }
    
    Out.vBackBuffer = vColor / vSize;
    
    return Out;
}

PS_OUT_BLUR_X PS_MAIN_METABALL_BLUR_X(PS_IN In)
{
    PS_OUT_BLUR_X Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    float4 vWeight = 0.f;
    float vSize;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX * 0.3;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 6] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
        vWeight += g_fWeights[i + 6] * g_WeightTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fWeights[i + 6];
    }
    
    Out.vBlurX = vColor / vSize;
    Out.vWeight = vWeight / vSize;
    
    return Out;
}

PS_OUT_BLUR_FINAL PS_MAIN_METABALL_BLUR_FINAL(PS_IN In)
{
    PS_OUT_BLUR_FINAL Out;
    
    float2 vTexcoord;
    float4 vColor;
    float4 vWeight = 0.f;
    float vSize;
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_iWinSizeY * 0.3;
        
        vColor += g_fWeights[i + 6] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
        vWeight += g_fWeights[i + 6] * g_WeightTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fWeights[i + 6];

    }
    
    Out.vBlurY = vColor / vSize;
    Out.vWeight = (vWeight / vSize);
    float fAlpha = saturate(Out.vWeight.g);
    Out.vWeight.g = saturate(pow(saturate(Out.vWeight.g * 2), 5));
    if (0 < fAlpha)
    {
        Out.vWeight *= Out.vWeight.g / fAlpha;
    }
    
    return Out;
}

technique11 DefaultTechnique
{ 
    // idx 0
    pass Blur_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }
    // idx 1
    pass Blur_Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_FINAL();
    }
    // idx 2
    pass Emissive_Blur_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE_BLUR_X();
    }
    // idx 3
    pass Emissive_Blur_Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE_BLUR_FINAL();
    }
    // idx 4
    pass MetaBall_Blur_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_METABALL_BLUR_X();
    }
    // idx 5
    pass MetaBall_Blur_Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_METABALL_BLUR_FINAL();
    }
}