#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;
float g_fFar;

matrix g_OrthoWorldMatrix, g_OrthoViewMatrix, g_OrthoProjMatrix;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_DepthTexture;
texture2D g_NormalTexture;
texture2D g_SSAOTexture;
texture2D g_SSAOBlurXTexture;

float3 g_NoiseValue[16];

//0.2 ~ 1.5
float g_fRadiusMin, g_fRadiusMax;
//0.001  ~ 1 
float g_fBiasMin, g_fBiasMax;
//0 ~ 3
float g_fIntensity;

/* 미리 생성된 커널 랜덤 노멀들 */
float3 g_vRandom[16] =
{
    float3(0.5381f, 0.1856f, 0.4319f), float3(0.1379f, 0.2486f, 0.4430f),
    float3(0.3371f, 0.5679f, -0.0057f), float3(-0.6999f, -0.0451f, 0.0019f),
    float3(0.0689f, -0.1598f, -0.8547f), float3(0.0560f, 0.0069f, -0.1843f),
    float3(-0.0146f, 0.1402f, 0.0762f), float3(0.0100f, -0.1924f, -0.0344f),
    float3(-0.3577f, -0.5301f, -0.4358f), float3(-0.3169f, 0.1063f, 0.0158f),
    float3(0.0103f, -0.5869f, 0.0046f), float3(-0.0897f, -0.4940f, 0.3287f),
    float3(0.7119f, -0.0154f, -0.0918f), float3(-0.0533f, 0.0596f, -0.5411f),
    float3(0.0352f, -0.0631f, 0.5460f), float3(-0.4776f, 0.2847f, -0.0271f)
};

float3 Calc_ViewSpace(float fDepth, float2 vTexCoord)
{
    vTexCoord.y = 1 - vTexCoord.y;
    vTexCoord = vTexCoord * 2.f - 1.f;
    
    //NDC 상의 공간.
    float4 vScreenPos = float4(vTexCoord, fDepth, 1.f);
    float4 vViewSpace = mul(vScreenPos, g_ProjMatrixInv);
    
    /* 
    왜 투영 행렬의 역행렬 곱하기 전에 w 곱하기 안함? -> 
    NDC 좌표만으로는 w값을 알 수 없는 상태라서, 일단 투영행렬 역행렬 곱하면
    w값이 살아남. -> 살아난 w값으로 나누기 처리.
    
    어렵다면 머릿속으로 

    뷰 스페이스 좌표 -> 투영행렬 곱 -> w 나누기 -> NDC.
    NDC에 투영 역행렬을 곱하면 다시 w값이 살아나서 해당 w값으로 나누기를 수행한다.

    외우셈..
    */
    return vViewSpace.xyz / vViewSpace.w;
}

float2 Calc_UV(float3 vViewSpacePosition)
{
    float4 vSamplePosScreen = mul(float4(vViewSpacePosition, 1.f), g_ProjMatrix);
    
    // z는 0 ~ 1, x,y는 -1 ~ 1로 고정.
    vSamplePosScreen.xyz /= vSamplePosScreen.w;
    // x,y는 0 ~ 1로 (UV값으로) 변환.
    vSamplePosScreen.x = vSamplePosScreen.x * 0.5f + 0.5f;
    vSamplePosScreen.y = 1.f - vSamplePosScreen.y;
    
    return vSamplePosScreen.xy;
}

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;   
  
    matrix matWV, matWVP;
    
    matWV = mul(g_OrthoWorldMatrix, g_OrthoViewMatrix);
    matWVP = mul(matWV, g_OrthoProjMatrix);   
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_SSAO(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    Out.vBackBuffer = float4(1.f, 1.f, 1.f, 1.f);
    
    int iSampleCount = 16;
    
    vector vDepth = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fDepth = vDepth.r;
    float fViewDepth = vDepth.g * g_fFar;
    
    float fRadius = lerp(g_fRadiusMin, g_fRadiusMax, saturate(vDepth.g));
    
    if (vDepth.r == 0.f && vDepth.g == 1.f && vDepth.b == 0.f && vDepth.a == 0.f)
        return Out;

    float3 vViewPos = Calc_ViewSpace(fDepth, In.vTexcoord);
    
    float3 vNormal = normalize(g_NormalTexture.Sample(DefaultSampler, In.vTexcoord).xyz * 2.f - 1.f);
    // 뷰 공간으로 노멀 변환
    vNormal = normalize(mul(vNormal, (float3x3) g_ViewMatrix));

    // TBN 생성. 
    int iX = (int) (In.vTexcoord.x * g_iWinSizeX) % 4;
    int iY = (int) (In.vTexcoord.y * g_iWinSizeY) % 4;
    int iIdx = iY * 4 + iX;
    
    float3 vRandomVec = g_NoiseValue[iIdx];
    float3 vTangent = normalize(vRandomVec - vNormal * dot(vRandomVec, vNormal));
    float3 vBitangent = cross(vNormal, vTangent);
    // TBN -> Normal 좌표계로 변환해주는 역할을 한다.
    float3x3 TBN = float3x3(vTangent, vBitangent, vNormal);

    float fOcclusion = 0.f;

    for (int i = 0; i < iSampleCount; ++i)
    {
        float3 vSample = g_vRandom[i];
        
        // 반구 영역으로 변환
        if (vSample.z < 0.0f)
        {
            vSample.z = -vSample.z;
        }

        //Sample Pos를 노멀좌표계 안의 반구 영역으로 끌어들인다.
        float3 vSamplePos = mul(vSample, TBN);
        vSamplePos = vViewPos + vSamplePos * fRadius;
        
        // 투영 행렬 곱
        float4 vOffset = float4(vSamplePos, 1.0f);
        vOffset = mul(vOffset, g_ProjMatrix);
        vOffset.xy /= vOffset.w;
        
        float2 vOffsetUV = vOffset.xy * 0.5f + 0.5f;
        vOffsetUV.y = 1.0f - vOffsetUV.y;
        
        if(vOffsetUV.x < 0 || vOffsetUV.x > 1.f || vOffsetUV.y < 0 || vOffsetUV.y > 1.f)
            continue;
        
        float fSampleDepthNDC = g_DepthTexture.Sample(DefaultSampler, vOffsetUV).r;
        float3 vGeoPos = Calc_ViewSpace(fSampleDepthNDC, vOffsetUV);
        float fGeoZ = vGeoPos.z; //실제 geometry view-space Z

        float fDeltaZ = abs(vViewPos.z - fGeoZ) + 0.001f;

        // Range Check 수정
        float fRangeCheck = 1.0f - saturate(fDeltaZ / fRadius);

        // bias는 고정값 권장
        float fBias = g_fBiasMin; // 또는 0.01f;

        // occlusion 조건
        if (fGeoZ <= vSamplePos.z - fBias)
            fOcclusion += fRangeCheck;
    }
    
    // 샘플링 평균
    fOcclusion = 1.0f - (fOcclusion / iSampleCount);

    fOcclusion = pow(fOcclusion, g_fIntensity);
    
    Out.vBackBuffer = float4(fOcclusion, fOcclusion, fOcclusion, 1.f);
    return Out;
}


PS_OUT_BACKBUFFER PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_iWinSizeX;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 6] * g_SSAOTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBackBuffer = vColor / 6.28f;
    
    //Out.vBackBuffer = g_SSAOTexture.Sample(ClampSampler, In.vTexcoord);
    
    return Out;
}

// 블러 y까지 처리하고 내보내기
PS_OUT_BACKBUFFER PS_MAIN_COMBINE(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float2 vTexcoord;
    float4 vBlurColor = 0.f;

    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_iWinSizeY;
        
        vBlurColor += g_fWeights[i + 6] * g_SSAOBlurXTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBackBuffer = vBlurColor / 6.28f;
    
    //Out.vBackBuffer = g_SSAOBlurXTexture.Sample(ClampSampler, In.vTexcoord);
    
    return Out;
}

technique11 DefaultTechnique
{ 
    // idx 0
    pass SSAO
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SSAO();
    }

    pass BlurX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
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