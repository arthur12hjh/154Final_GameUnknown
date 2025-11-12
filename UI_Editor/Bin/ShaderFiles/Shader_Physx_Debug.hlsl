#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

VS_PHYSX_OUT VS_MAIN_PHYSX(VS_PHYSX_IN In)
{
    VS_PHYSX_OUT Out;
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vColor = In.vColor;
    
    return Out;
}

PS_PHYSX_OUT PS_MAIN_PHYSX(VS_PHYSX_OUT In)
{
    PS_PHYSX_OUT Out;
    Out.vColor = In.vColor;
    
    return Out;
}


technique11 DefaultTechnique
{
    pass Physx
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_PHYSX();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PHYSX();
    }
}
