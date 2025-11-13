#include "Engine_Shader_Defines.hlsli"

inline float4 Calc_Shadow(float4 vBackBuffer, texture2D ShadowTexture, vector vPosition)
{ 
    float2 vTexcoord;
    vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;

    float fSum = 0.0f;
    
    float2 fTexelSize = 1.0f / float2(8192.0f, 4608.0f) * 0.5f; // 그림자맵 해상도에 맞게 조정

    // PCF 3x3 샘플
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float2 offset = float2(x, y) * fTexelSize;
            float fShadowDepth = ShadowTexture.Sample(DefaultSampler, vTexcoord + offset).x * 500.0f;
            fSum += (vPosition.w - 0.1f > fShadowDepth) ? 1.0f : 0.0f;
        }
    }

    fSum /= 9.0f; // 평균 (3x3)
    vBackBuffer *= lerp(1.0f, 0.6f, fSum); // 그림자 강도 적용

    return vBackBuffer;
}

inline float4 Calc_Blur(texture2D BlurTexture, float2 vTexcoord)
{
    // 블러 연산
    float4 vBlurColor = BlurTexture.Sample(DefaultSampler, vTexcoord);
    
    return vBlurColor;
}

inline float4 Calc_Glow(texture2D GlowTexture, float2 vTexcoord)
{
    // 블러 연산
    float4 vGlowColor = GlowTexture.Sample(DefaultSampler, vTexcoord);
    
    return vGlowColor;
}

inline float4 Calc_Outline(float4 vBackBuffer, texture2D OutlineTexture, float2 vTexcoord)
{
    float4 vOutline = OutlineTexture.Sample(DefaultSampler, vTexcoord);
    
    float4 vResult = lerp(vBackBuffer * vOutline, vBackBuffer, 0.4f);
    return vResult;
}

/* 배럴 왜곡 이용해서 구현한거임. 내부 로직 바꾸고 싶으면 일단 디코로 따로 말해줘*/
inline float4 Calc_Distortion(texture2D SceneTexture, texture2D DistortionTexture, float2 vTexcoord)
{   
    /* 씬 텍스쳐 샘플링 */
    float4 vScene = SceneTexture.Sample(DefaultSampler, vTexcoord);
    
    /* 디스토션 렌더타겟 텍스쳐 샘플링 */
    float4 vDistortion = DistortionTexture.Sample(DefaultSampler, vTexcoord);
    
    /* r 성분을 디스토션할 성분으로 잡아놨으니까, r이 0이라면 리턴. */
    if(vDistortion.r == 0)
        return vScene;
 
    // 0 ~ 1 -> -0.5 ~ 0.5
    float2 vCenteredUV = vTexcoord - float2(0.5f, 0.5f);
    float fDistortionFactor = 1 + vDistortion.g * vDistortion.r * vDistortion.r;
    //너무 낮은값 쓰면 찢어지는 효과 남. (같은 픽셀 계속 디스토션 돌려서)
    fDistortionFactor = clamp(fDistortionFactor, 0.2f, 2.0f);

    vCenteredUV *= fDistortionFactor;
    vCenteredUV += float2(0.5f, 0.5f);
    
    float4 vResult = SceneTexture.Sample(MirrorSampler, vCenteredUV);

    // 원래 색과 섞어.
    //float fFade = (vDistortion.r > 0.0f) ? 1.0f : 0.0f;
    //vResult.rgb = lerp(vScene.rgb, vResult.rgb, 0.8f * fFade);
    
    return vResult;
}