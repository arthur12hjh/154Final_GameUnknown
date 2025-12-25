#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;
float g_fFar;

float g_fTimeAcc;
float g_fLifeTime;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_PreViewMatrix, g_PreProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_PreViewMatrixInv, g_PreProjMatrixInv; 

texture2D g_SceneTexture;
//Velocity Texture가 기록되었다고 가정. 
texture2D g_VelocityTexture;
texture2D g_CamVelocityTexture;

//깊이 맵은 가져다 쓰자..
texture2D g_DepthTexture;

float2 g_vCamVelocity;

float g_fCamBlurScale;
float g_fObjectBlurScale; 

float g_fBias;
unsigned int   g_iSampleCount; 

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

PS_OUT_BACKBUFFER PS_MAIN_CAM_MOTIONBLUR(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    return Out;
}

/* 방사형 블러 이전에 추가할 것.. */
/* 클라단에서 Velocity만 기록해주면 된다. */
PS_OUT_BACKBUFFER PS_MAIN_MotionBlur(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    float4 baseColor = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);

    // 분리된 velocity 읽기
    float4 vObjVelocity = g_VelocityTexture.Sample(ClampSampler, In.vTexcoord);

    // 최종 velocity = object + camera
    float2 vVelocity = vObjVelocity.xy;

    float2 vSampledVelocity = vVelocity / g_iSampleCount;

    float fCurrentDepth = g_DepthTexture.Sample(ClampSampler, In.vTexcoord).g * g_fFar;

    float4 accum = baseColor;
    float count = 1.0f;

    float2 vStepVelocity = vObjVelocity / g_iSampleCount * g_fObjectBlurScale;

    [loop]
    for (int i = 1; i < g_iSampleCount; ++i)
    {
        float2 offset = In.vTexcoord + vStepVelocity * i;
        if (offset.x < 0 || offset.x > 1 || offset.y < 0 || offset.y > 1)
            continue;

        float sampleDepth = g_DepthTexture.Sample(ClampSampler, offset).g * g_fFar;

        if (fCurrentDepth <= sampleDepth + g_fBias)
        {
            float4 sampleColor = g_SceneTexture.Sample(ClampSampler, offset);
            float w = 1.0f - (float) i / g_iSampleCount;

            accum += sampleColor * w;
            count += w;
        }
    }
    
    Out.vBackBuffer = accum / count;
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
    pass CamMotionBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CAM_MOTIONBLUR();
    }
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