#include "Client_Shader_Utils.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vColor;

texture2D g_SceneTexture;

struct VS_IN_DEFERRED
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT_DEFERRED
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_IN_DEFERRED
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_DEFERRED
{
    float4 vBackBuffer : SV_TARGET0;
};

VS_OUT_DEFERRED VS_MAIN(VS_IN_DEFERRED In)
{
    VS_OUT_DEFERRED Out;
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

PS_OUT_DEFERRED PS_MAIN(PS_IN_DEFERRED In)
{
    PS_OUT_DEFERRED Out;
    
    Out.vBackBuffer = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);
    //¿œ¥‹ ∫”∞‘ «ÿ∫Ω
    Out.vBackBuffer *= float4(1.f, 0.2f, 0.2f, 1.f);
    
    return Out;   
}

technique11 DefaultTechnique
{ 
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}