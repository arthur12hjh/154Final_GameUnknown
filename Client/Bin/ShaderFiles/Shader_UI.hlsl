#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture;
texture2D g_DepthTexture;

vector g_Color = 1.f;

float2 g_UIPosition;
float2 g_UISize;
vector g_UIDebugLineColor;

float g_Alpha = 1.f;

/*------------------[S_DEBUG]---------------*/

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
    
    Out.vColor *= g_Alpha;
    
    if (Out.vColor.a <= 0.05f)
        discard;
    
    return Out;
}

/*------------------[E_DEFAULT]---------------*/


/*------------------[S_DEBUG]---------------*/

struct VS_IN_DEBUG
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT_DEBUG
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT_DEBUG VS_MAIN_DEBUG(VS_IN_DEBUG In)
{
    VS_OUT_DEBUG Out;
    
    Out.vPosition = float4(g_UIPosition, 0, 1);
    Out.vTexcoord = float2(0, 0); // 필요 없음
    
    return Out;
}

struct GS_IN_DEBUG
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct GS_OUT_DEBUG
{
    float4 vPosition : SV_POSITION;
};

[maxvertexcount(6)]
void GS_MAIN_DEBUG(point GS_IN_DEBUG In[1], inout TriangleStream<GS_OUT_DEBUG> OutStream)
{
    GS_OUT_DEBUG Out[4];
    
    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);
    
    Out[0].vPosition = mul(float4(-0.5f, 0.5f, 0.f, 1.f), matWVP);
    ///Out[0].vTexcoord = float2(0.f, 0.f);
    
    Out[1].vPosition = mul(float4(0.5f, 0.5f, 0.f, 1.f), matWVP);
    //Out[1].vTexcoord = float2(1.f, 0.f);
    
    Out[2].vPosition = mul(float4(0.5f, -0.5f, 0.f, 1.f), matWVP);
    //Out[2].vTexcoord = float2(1.f, 1.f);
    
    Out[3].vPosition = mul(float4(-0.5f, -0.5f, 0.f, 1.f), matWVP);
    //Out[3].vTexcoord = float2(0.f, 1.f);
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}

//struct PS_IN_DEBUG
//{
//    float2 vPosition : POSITION;
//};
//
//struct PS_OUT_DEBUG
//{
//    float4 vColor : SV_TARGET0;
//};

float4 PS_MAIN_DEBUG() : SV_TARGET
{
    return g_UIDebugLineColor;
}

/*------------------[E_DEBUG]---------------*/

technique11 DefaultTechnique
{
    pass UI
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Debug
    {
        SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_DEBUG();
        GeometryShader = compile gs_5_0 GS_MAIN_DEBUG();
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass UI_Blend
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(1.f, 1.f, 1.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}