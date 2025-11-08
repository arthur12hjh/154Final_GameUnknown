#include "Engine_Shader_Defines.hlsli"

inline float4 Calc_Shadow(float4 vBackBuffer, texture2D ShadowTexture, vector vPosition)
{ 
    float2 vTexcoord;
    
    vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;
    
    vector vShadowDepth = ShadowTexture.Sample(DefaultSampler, vTexcoord);
    
    if (vPosition.w - 0.1f > vShadowDepth.x * 500.0f)
        vBackBuffer *= 0.5f;
    
    return vBackBuffer;
}

inline float4 Calc_Blur(texture2D BlurTexture, float2 vTexcoord)
{
    // 블러 연산
    float4 vBlurColor = BlurTexture.Sample(DefaultSampler, vTexcoord);
    
    return vBlurColor;
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
    fDistortionFactor = clamp(fDistortionFactor, 0.5f, 1.5f);

    vCenteredUV *= fDistortionFactor;
    vCenteredUV += float2(0.5f, 0.5f);
    
    float4 vResult = SceneTexture.Sample(MirrorSampler, vCenteredUV);

    // 원래 색과 섞어.
    float fFade = smoothstep(0.0f, 0.3f, vDistortion.r);
    vResult.rgb = lerp(vScene.rgb, vResult.rgb, 0.8f * fFade);
    
    return vResult;
}