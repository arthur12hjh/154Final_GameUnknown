struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_BACKBUFFER
{
    float4 vBackBuffer : SV_TARGET0;
};

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};

struct PS_OUT_BLUR_X
{
    float4 vBlurX : SV_TARGET0;
};

struct PS_OUT_BLUR_FINAL
{
    float4 vBlurY : SV_TARGET0;
};

struct PS_OUT_DISTORTION
{
    float4 vDistortion : SV_TARGET0;
};

/*
블러를 위한 가우시안 계수. 
추후 수정 필요.
*/

float g_fWeights[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};