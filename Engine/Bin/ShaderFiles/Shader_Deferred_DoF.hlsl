#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_SceneTexture;
texture2D g_SceneBlurXTexture;
texture2D g_DepthTexture;

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

PS_OUT_BACKBUFFER PS_MAIN_SCREEN_BLUR_X(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 6] * g_SceneTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBackBuffer = vColor / 6.28f;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_DOF(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float2 vTexcoord;
    float4 vBlurColor = 0.f;
    
    Out.vBackBuffer = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vDepthDesc.r == 0 && vDepthDesc.g == 1 && vDepthDesc.b == 0 && vDepthDesc.a == 0)
        return Out;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_iWinSizeY;
        
        vBlurColor += g_fWeights[i + 6] * g_SceneBlurXTexture.Sample(ClampSampler, vTexcoord);
    }
    
    vBlurColor = vBlurColor / 6.28f;
    
    // 뷰 스페이스 상의 z. 카메라로부터 떨어진 거리를 의미하니까..
    float fViewZ = vDepthDesc.y * 500.f;
    vector vPosition;

    float fFocusDistance = 15.0f; // 카메라 초점 거리
    float fMaxRange = 300.f;      // 블러가 최대로 적용될 거리
    float fBlurAmount = saturate(abs(fViewZ - fFocusDistance) / fMaxRange);
    
    Out.vBackBuffer = lerp(Out.vBackBuffer, vBlurColor, fBlurAmount);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINE(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    Out.vBackBuffer = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

technique11 DefaultTechnique
{ 
    // idx 0
    pass BlurX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SCREEN_BLUR_X();
    }

    //기존 렌더타겟으로 합성해서 내보내기 위한 렌더타겟.
    pass DoF
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DOF();
    }

    //기존 렌더타겟으로 합성해서 내보내기 위한 렌더타겟.
    pass Combine
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINE();
    }
}