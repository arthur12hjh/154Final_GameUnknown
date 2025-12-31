#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D   g_Texture;

struct VS_IN
{
    float3 vPosition : POSITION;  
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float3 vScale : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
  
    Out.vPosition = vector(In.vPosition, 1.f);
    Out.vScale = float3(length(g_WorldMatrix._11_12_13_14), length(g_WorldMatrix._21_22_23_24), length(g_WorldMatrix._31_32_33_34));
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float3 vScale : TEXCOORD0;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];
    
    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);
    
    Out[0].vPosition = mul(float4(-0.5f, 0.5f, 0.f, 1.f), matWVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    
    Out[1].vPosition = mul(float4(0.5f, 0.5f, 0.f, 1.f), matWVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    
    Out[2].vPosition = mul(float4(0.5f, -0.5f, 0.f, 1.f), matWVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    
    Out[3].vPosition = mul(float4(-0.5f, -0.5f, 0.f, 1.f), matWVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;    
    
    vector vTexture = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vTexture.a < 0.3f)
        discard;
    
    Out.vColor = vTexture;
    return Out;   
}

technique11 DefaultTechnique
{ 
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}