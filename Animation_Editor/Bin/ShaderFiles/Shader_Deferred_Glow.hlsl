#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;
float g_fFar;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_GlowTexture;
texture2D g_GlowPowerTexture;


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

PS_OUT_BLUR PS_MAIN_GLOW_X(PS_IN In)
{
    PS_OUT_BLUR Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    float vSize = 0.f;
    
    for (int i = -10; i < 11; ++i)
    {
        
        vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fBloomWeights[i + 10] * g_GlowTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fBloomWeights[i + 10];
    }

    Out.vBlur = vColor / vSize * 2;
    return Out;
}

PS_OUT_BLUR PS_MAIN_GLOW_FINAL(PS_IN In)
{
    PS_OUT_BLUR Out;
    float2 vTexcoord;
    float4 vColor = 0.f;
    float vSize = 0.f;
    
    for (int i = -10; i < 11; ++i)
    {
        
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_iWinSizeY;
    
        vColor += g_fBloomWeights[i + 10] * g_GlowTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fBloomWeights[i + 10];
    }
    
    for (int i = -6; i < 7; ++i)
    {
    }
    vColor /= vSize;
    float3 color = g_GlowPowerTexture.Sample(ClampSampler, In.vTexcoord).rgb;
    if (0.7 < max(color.r, max(color.g, color.b)))
        vColor.rgb += max((g_GlowPowerTexture.Sample(ClampSampler, In.vTexcoord).rgb * 6) - vColor.rgb, 0);
    if (0 >= vColor.a)
        discard;
    Out.vBlur = vColor;
    return Out;
}

PS_OUT_BLUR PS_MAIN_GLOW_CORE(PS_IN In)
{
    PS_OUT_BLUR Out;
    float2 vTexcoord;
    float4 vColor = g_GlowTexture.Sample(ClampSampler, In.vTexcoord);
    if (0 >= vColor.a)
    {
        //for (int i = -1; i < 2; ++i)
        //{
        //    vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX;
        //    for (int j = -1; j < 2; ++j)
        //    {
        //        vTexcoord.y = In.vTexcoord.y + (float) j / g_iWinSizeY;
        //        vColor = g_GlowTexture.Sample(ClampSampler, vTexcoord);
        //        if (0 < vColor.a)
        //        {
        //            if (1 == abs(i))
        //                vColor.a *= 0.5;
        //            if (1 == abs(j))
        //                vColor.a *= 0.5;
        //            break;
        //        }
        //    }
        //    if (0 < vColor.a)
        //        break;
        //}
        if (0 >= vColor.a)
            discard;
    }
    Out.vBlur = float4(saturate(vColor.rgb * 6), vColor.a);
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
    pass Glow_Core
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLOW_CORE();
    }
}