#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;
float g_fFar;

float g_fTimeAcc;
float g_fLifeTime;
int g_iSampleCount;
float g_fSamplePower;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_SceneTexture;

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

PS_OUT_BACKBUFFER PS_MAIN_RADIAL_BLUR(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
 
    float4 vColor = 0.0f;
    
    float2 vDir = float2(0.5f, 0.5f) - In.vTexcoord;
    vDir *= length(float2(0.5f, 0.5f) - In.vTexcoord) * g_fSamplePower;
    
    [loop]
    for (int i = 0; i < g_iSampleCount; i++)
    {
        float fSampleRate = (float) i / (float) g_iSampleCount;
        
        // 샘플링 UV 좌표. 현재 UV + 방향 벡터 * 계수
        float2 vSampleUV = In.vTexcoord + vDir * fSampleRate * (-4 * pow(saturate(g_fTimeAcc / g_fLifeTime), 2.f) + 4 * saturate(g_fTimeAcc / g_fLifeTime));
        
        // 텍스처 샘플링 및 누적
        vColor += g_SceneTexture.Sample(DefaultSampler, vSampleUV);
    }
    
    Out.vBackBuffer = vColor / (float) g_iSampleCount;

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
    pass RadialBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RADIAL_BLUR();
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