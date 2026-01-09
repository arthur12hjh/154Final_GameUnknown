vector g_vMtrlDiffuse = 1.f;
vector g_vMtrlAmbient = 1.f;
vector g_vMtrlSpecular = 1.f;

#define MIP_BIAS 3.f

struct BoneTransformMatrix
{
    row_major float4x4 BoneLocalTransformMatrix;
    row_major float4x4 BoneCombinedTransformMatrix;
};

SamplerComparisonState ShadowSampler
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT; // PCF 핵심
    AddressU = Clamp;
    AddressV = Clamp;
    ComparisonFunc = LESS_EQUAL;
};

sampler AnisoTropy_BLUR_Sampler = sampler_state // 밉맵이 있어야 의미가 있음
{
    Filter = AnisoTropic;
    MaxAnisotropy = 16;
    AddressU = wrap;
    AddressV = wrap;
    AddressW = wrap;
    MipLodBias = 4.f; // 밉맵 레벨 +1시켜서 의도적으로 뭉갬
    MinLOD = 0;
    MaxLOD = 0xFFFFFFFF;
    ComparisonFunc = ALWAYS;
};

sampler NormalSampler = sampler_state
{
    Filter = ANISOTROPIC; // 또는 MIN_MAG_ANISOTROPIC_MIP_LINEAR 등
    AddressU = wrap;
    AddressV = wrap;
    MaxAnisotropy = 16;
};

sampler ClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = clamp;
    AddressV = clamp;
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};


sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

SamplerState Noise3DSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};

sampler MirrorSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = mirror;
    AddressV = mirror;
};

sampler NoneSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = border;
    AddressV = border;
    BorderColor = float4(0, 0, 0, 0);
};

RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
};

RasterizerState RS_Wireframe
{
    FillMode = Wireframe;
};

RasterizerState RS_Cull_Front
{
    CullMode = Front;
};

/* Cull 모드 끄기. */
RasterizerState RS_Cull_None
{
    CullMode = None;
};

/* DSS 기본 설정 */
DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

/* 깊이 쓰기 & 깊이 테스트 끄기  */
DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;    
};

/* 깊이쓰기만 끄기*/
DepthStencilState DSS_DepthNonWrite
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less_equal;
};

/* 블렌더 끄기. 어떤 렌더타겟에 블렌드 스테이트를 넘겨줄 것인지 설정. */
BlendState BS_None
{
    BlendEnable[0] = false;
 
};

/* 블렌더 켜기. 어떤 렌더타겟에 블렌드 스테이트를 넘겨줄 것인지 설정. */
BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};

/* One, One, Add 모드로 블렌드 켜기.*/
BlendState BS_Blend
{
    BlendEnable[0] = true;    
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};

/* alpha 값도 One, One, Add 모드 켜기 */
BlendState BS_BlendAlpha
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = add;
    SrcBlendAlpha = one;
    DestBlendAlpha = one;
    BlendOpAlpha = add;
};


/* One, One, Add 모드로 블렌드 켜기.*/
BlendState BS_Override
{
    BlendEnable[0] = true;
    BlendEnable[1] = false;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};

DepthStencilState DSS_DepthTest_ON_Write_OFF
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO; // 깊이 버퍼 쓰기 끔
};
