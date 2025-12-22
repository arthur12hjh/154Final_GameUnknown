#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;
float g_fFar;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_BlendTexture;


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

PS_OUT_BLUR PS_BLACK_BLEND_WEIGHT(PS_IN In)
{
    PS_OUT_BLUR Out;
    float4 BlurColor = g_BlendTexture.Sample(ClampSampler, In.vTexcoord);
    if (0 >= BlurColor.a)
        discard;
    Out.vBlur.rgb = BlurColor.rgb / BlurColor.a;
    Out.vBlur.a = saturate(BlurColor.a);
    return Out;
}

technique11 DefaultTechnique
{
    // idx 0
    pass Black_Blend_Weight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLACK_BLEND_WEIGHT();
    }
}