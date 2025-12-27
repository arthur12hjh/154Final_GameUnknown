#include "Client_Shader_Utils.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture;
texture2D g_DepthTexture;

float g_fDistortionIntensity;
float g_fFar;

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

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    
    /* In.vPosition * 월드 * 뷰 * 투영 */    
    //float4x4 == matrix
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    
    /* Out.vPosition.xy => 시야각에 있는 점들을 90에 맞춰준다 */ 
    /* Out.vPosition.z => n~f사이에 있는 점들의 z를 0 ~ f로 바꿔준다. */     
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct VS_OUT_SOFTEFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT_SOFTEFFECT VS_MAIN_SOFTEFFECT(VS_IN In)
{
    VS_OUT_SOFTEFFECT Out;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    
    /* Out.vPosition.xy => 시야각에 있는 점들을 90에 맞춰준다 */ 
    /* Out.vPosition.z => n~f사이에 있는 점들의 z를 0 ~ f로 바꿔준다. */     
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}

/* 출력된 정점 위치벡터의 w값으로 모든 성분을 나눈다 -> 투영스페이스로 변환 */ 
/* 정점의 위치에 대해서 뷰포트 변환을 수행한다 */ 
/* 정점의 모든 정보를 보간하여 픽셀을 만든다. -> 래스터라이즈 */ 

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    if (Out.vColor.a == 0.f)
        discard;
    
    return Out;
}

struct PS_IN_SOFTEFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

PS_OUT PS_MAIN_SOFTEFFECT(PS_IN_SOFTEFFECT In)
{
    PS_OUT Out;
    
    float4 vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vColor.a == 0.f)
        discard;
    
    float2 vTexcoord;
    
    vTexcoord.x = In.vProjPos.x / In.vProjPos.w * 0.5f + 0.5f;
    vTexcoord.y = In.vProjPos.y / In.vProjPos.w * -0.5f + 0.5f;
    
    float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexcoord);
    
    float fOldViewZ = vDepthDesc.y * g_fFar;
    
    float fDistance = fOldViewZ - In.vProjPos.w;
    
    vColor.a = vColor.a * saturate(fDistance);
    
    Out.vColor = vColor;
    
    return Out;
}

PS_OUT PS_DISTORTION(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Out.vColor.a == 0.f)
        discard;
    /* g,b,a 성분은 0으로 밀어준다. */
    Out.vColor.ba = 0.f;
    /* 거리 기반으로 디스토션 할 객체 중점으로부터의 거리 구해냄. */
    /* 중점이라면 r은 1, 중점에서의 거리가 멀수록 1에서 r 성분의 크기가 줄어드는 형태. */
    Out.vColor.r = 1 - length(In.vTexcoord - float2(0.5f, 0.5f));
    
    /* */
    Out.vColor.g = g_fDistortionIntensity;
    
    return Out;
}

struct VS_OUT_MOTIONBLUR
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vDirection : TEXCOORD1;
};

struct PS_IN_MOTIONBLUR
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vDirection : TEXCOORD1;
};

struct PS_OUT_MOTIONBLUR
{
    float4 vDirection : SV_TARGET0;
};

VS_OUT_MOTIONBLUR VS_MAIN_VELOCITY(VS_IN In)
{
    VS_OUT_MOTIONBLUR Out;
    
    
    /* In.vPosition * 월드 * 뷰 * 투영 */    
    //float4x4 == matrix
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vDirection = float4(0.f, 0.f, 0.f, 0.f);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

PS_OUT_MOTIONBLUR PS_MAIN_VELOCITY(PS_IN_MOTIONBLUR In)
{
    PS_OUT_MOTIONBLUR Out;

    //바깥쪽으로 뻗어나가는 벨로시티가 찍힐 것
    Out.vDirection.xy = -1.f * (float2(0.5f, 0.5f) - In.vTexcoord) * 0.1f;
    Out.vDirection.zw = float2(0.f, 0.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    // 0
    pass UI
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    // 1
    pass SoftEffect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_SOFTEFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SOFTEFFECT();
    }
    // 2
    pass Blur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    // 3
    pass Distortion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthTest_ON_Write_OFF, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }
    //4 
    pass TestVelocity
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthTest_ON_Write_OFF, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_VELOCITY();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_VELOCITY();
    }
}