/* 블러를 위한 가우시안 계수. 추후 수정 필요. */

float g_fWeights[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

float g_fBloomWeights[21] =
{
    0.0125f, 0.0249f, 0.0469f, 0.0818f, 0.1332f, 0.2052f, 0.3019f, 0.4259f, 0.5741f, 0.7370f, 1.0f,
    0.7370f, 0.5741f, 0.4259f, 0.3019f, 0.2052f, 0.1332f, 0.0818f, 0.0469f, 0.0249f, 0.0125f
};

/* 툰 셰이딩을 위한 라플라시안 필터 계수. 추후 수정할수도 있음 */
float g_fLaplacianMask[9] =
{
    -1, -1, -1,
    -1, 8, -1,
    -1, -1, -1,
};

float g_fPixelsX[9] =
{
    -1.f, 0.f, 1.f,
    -1.f, 0.f, 1.f,
    -1.f, 0.f, 1.f
};
float g_fPixelsY[9] =
{
    -1.f, -1.f, -1.f,
    0.f, 0.f, 0.f,
    1.f, 1.f, 1.f
};


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

struct PS_OUT_GLOW_X
{
    float4 vGlowX : SV_TARGET0;
};

struct PS_OUT_GLOW_FINAL
{
    float4 vGlowY : SV_TARGET0;
};


struct PS_OUT_DISTORTION
{
    float4 vDistortion : SV_TARGET0;
};

//피직스 디버깅용
struct VS_PHYSX_IN
{
    float3 vPosition : POSITION;
    float4 vColor : COLOR0;
};

struct VS_PHYSX_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR0;
};


struct PS_PHYSX_IN
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR0;
};

struct PS_PHYSX_OUT
{
    float4 vColor : SV_TARGET0;
};