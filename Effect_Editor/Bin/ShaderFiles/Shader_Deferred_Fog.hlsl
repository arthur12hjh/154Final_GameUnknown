#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;
float g_fFar;

float g_fFogStart, g_fFogEnd;
float g_fFogPowerMin, g_fFogPowerMax;
float g_fSkyBoxFogPower;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_DepthTexture;

//뎁스 텍스쳐 기반으로 포그 강도 기록해둘 렌더타겟

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

PS_OUT_FOG PS_MAIN_FOG(PS_IN In)
{
    PS_OUT_FOG Out;
    Out.fFogPower = 1.0f;
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    //깊이 기록 되지 않은 녀석들에게 처리해주기 위한 코드
    if (vDepthDesc.r == 0 && vDepthDesc.g == 1 && vDepthDesc.b == 0 && vDepthDesc.a == 0)
    {
        Out.fFogPower = g_fSkyBoxFogPower;
        return Out;
    }
    
    float fViewZ = vDepthDesc.y * g_fFar;
    vector vPosition;

    /* 로컬위치 * 월드 * 뷰 * 투영 / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    /* 로컬위치 * 월드 * 뷰 * 투영  */
    vPosition = vPosition * fViewZ;
    /* 로컬위치 * 월드 * 뷰  */
    vPosition = mul(vPosition, g_ProjMatrixInv);

    Out.fFogPower = clamp((g_fFogEnd - vPosition.z) / (g_fFogEnd - g_fFogStart), g_fFogPowerMin, g_fFogPowerMax);
    
    return Out;
}

technique11 DefaultTechnique
{ 
    // idx 0
    pass Fog
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FOG();
    }
}