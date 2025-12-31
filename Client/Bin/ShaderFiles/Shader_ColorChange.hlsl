#include "Client_Shader_Utils.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vColor;
float g_fTime;

texture2D g_SceneTexture, g_GlassTexture;

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
    
    //float4 vGlass = g_GlassTexture.SampleLevel(DefaultSampler, In.vTexcoord, 0);
    //vGlass = 1 - vGlass;
    //if (vGlass.r == 0)
    //    return Out;
    //
    //float2 vCenteredUV = In.vTexcoord - float2(0.5f, 0.5f);
    //float fDistortionFactor = 1 + vGlass.g * vGlass.r * vGlass.r;
    //fDistortionFactor = clamp(fDistortionFactor, 0.2f, 2.0f);
    //
    //vCenteredUV *= fDistortionFactor;
    //vCenteredUV += float2(0.5f, 0.5f);
    float2 center = float2(0.5, 0.5);

    float2 dir = In.vTexcoord - center;
    float len = length(dir);
    
    float crackMask = g_GlassTexture.SampleLevel(DefaultSampler, In.vTexcoord, 0).r;
    
    float strength = saturate(1 - len) * 0.1;

    float2 distortedUV = In.vTexcoord + -normalize(dir) * strength * crackMask * sin(g_fTime * 2.5);
    
    
    
    Out.vBackBuffer = g_SceneTexture.Sample(ClampSampler, distortedUV);
    Out.vBackBuffer = lerp(Out.vBackBuffer, 1 - Out.vBackBuffer, saturate(sin(-g_fTime * 2.5)));
    
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