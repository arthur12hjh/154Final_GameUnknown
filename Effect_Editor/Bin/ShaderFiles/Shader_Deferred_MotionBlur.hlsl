#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;

float g_fTimeAcc;
float g_fLifeTime;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

/* 현재는 WorldMatrix의 변화, 카메라의 변화에만 따라가도록 되어있음. */

texture2D g_SceneTexture;
//Velocity Texture가 기록되었다고 가정. 
texture2D g_VelocityTexture;
//깊이 맵은 가져다 쓰자..
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

/* 방사형 블러 이전에 추가할 것.. */
/* 클라단에서 Velocity만 기록해주면 된다. */
PS_OUT_BACKBUFFER PS_MAIN_MotionBlur(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

    float4 vBaseColor = g_SceneTexture.Sample(ClampSampler, In.vTexcoord);
    float4 vAccum = vBaseColor;

    int iSampleCount = 8;

    float4 vVelocity = g_VelocityTexture.Sample(ClampSampler, In.vTexcoord);
    vVelocity.xy /= (float) iSampleCount;

    float fCurrentDepth = g_DepthTexture.Sample(ClampSampler, In.vTexcoord).g * 500.f;

    int iCnt = 1;

    for (int i = iCnt; i < iSampleCount; i++)
    {
        float2 offset = In.vTexcoord + vVelocity.xy * (float) i * 0.1f;

        float4 vColor = g_SceneTexture.Sample(ClampSampler, offset);
        float fSampleDepth = g_DepthTexture.Sample(ClampSampler, offset).g * 500.f;

        if (fSampleDepth <= fCurrentDepth + 0.01f)
        {
            vColor.a = 0.6f; // 가중치 조절
            vAccum += vColor;
            iCnt++;
        }
    }

    Out.vBackBuffer = vAccum / iCnt;

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
    pass MotionBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MotionBlur();
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