#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_MetaballTexture;

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

PS_OUT_BLUR PS_METABALL_WEIGHT(PS_IN In)
{
    PS_OUT_BLUR Out;
    float4 MetaballColor = g_MetaballTexture.Sample(ClampSampler, In.vTexcoord);
    if (0 >= MetaballColor.a)
        discard;
    Out.vBlur.rgb = MetaballColor.rgb / MetaballColor.a;
    Out.vBlur.a = saturate(MetaballColor.a);
    
    float fakeLight = saturate(MetaballColor.a * 0.7);
    fakeLight = pow(fakeLight, 1.5);
    
    float lighting = lerp(0.6, 1.2, fakeLight);
    
    Out.vBlur.rgb *= lighting;

    
    
    
    
    return Out;
}

PS_OUT_BLUR PS_MAIN_METABALL_X(PS_IN In)
{
    PS_OUT_BLUR Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    float vSize = 0.f;
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 6] * g_MetaballTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fWeights[i + 6];
    }
    
    Out.vBlur = vColor / vSize;
    return Out;    
}

PS_OUT_BLUR PS_MAIN_METABALL_FINAL(PS_IN In)
{
    PS_OUT_BLUR Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    float vSize = 0.f;
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_iWinSizeY;
        
        vColor += g_fWeights[i + 6] * g_MetaballTexture.Sample(ClampSampler, vTexcoord);
        vSize += g_fWeights[i + 6];

    }
    
    Out.vBlur = vColor / vSize;
    
    float density = saturate(Out.vBlur.a);

    float threshold = 0.4;
    float soft = 0.1;
    
    Out.vBlur.a = smoothstep(threshold - soft,
                          threshold + soft,
                          density);
    return Out;
}

technique11 DefaultTechnique
{
    // idx 0
    pass MetaBall_Weight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_METABALL_WEIGHT();
    }
    // idx 1
    pass MetaBall_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_METABALL_X();
    }
    // idx 2
    pass MetaBall_Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_METABALL_FINAL();
    }
}