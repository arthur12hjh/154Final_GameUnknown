#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

float g_fFar;
int g_iWinSizeX;
int g_iWinSizeY;
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ProjMatrixInv, g_ViewMatrixInv;

Texture3D g_VolumetricFogTexture;
texture2D g_DepthTexture;
vector g_vCamPosition;

float g_fDepthPackExponent;
float g_fNearPlaneDist;
float g_fFarPlaneDist;

float ConvertDepthToNdcZ(float depth)
{
    float depthPackExponent = g_fDepthPackExponent;
    float nearPlaneDist = g_fNearPlaneDist;
    float farPlaneDist = g_fFarPlaneDist;

    return pow(saturate((depth - nearPlaneDist) / (farPlaneDist - nearPlaneDist)), 1 / depthPackExponent);
}


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

PS_OUT_VOLUME_FOG PS_MAIN_VOLUME_FOG(PS_IN In)
{
    PS_OUT_VOLUME_FOG Out;


    float4 vDepth = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);

    if (vDepth.r == 0 && vDepth.g == 1 && vDepth.b == 0 && vDepth.a == 0)
    {
        Out.vVolumeFogDesc = float4(0.f, 0.f, 0.f, 1.f);
        return Out;
    }
    
    float fViewDepth = vDepth.y * g_fFar;
    vector vViewPosition = GetViewPosition(g_DepthTexture, g_fFar, In.vTexcoord, g_ProjMatrixInv);
    
    Out.vVolumeFogDesc = g_VolumetricFogTexture.Sample(Noise3DSampler, float3(In.vTexcoord, ConvertDepthToNdcZ(fViewDepth)));

    
    return Out;
}

technique11 DefaultTechnique
{
    pass VolumeFog
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0, 0, 0, 0), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_VOLUME_FOG();
    }
}