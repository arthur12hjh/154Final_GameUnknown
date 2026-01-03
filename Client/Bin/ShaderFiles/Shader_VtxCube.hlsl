#include "Client_Shader_Utils.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
textureCUBE g_Texture;
float g_fFar;
float g_fColorWeight;
float g_fOutlineWidth = 0.02f;

/* 정점 쉐이더 : */
/* 정점에 대한 셰이딩 == 정점에 필요한 연산을 수행한다 == 정점의 상태변환(월드, 뷰, 투영) + 추가변환 */
/* 정점의 구성 정보를 수정, 변경한다 */ 
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;    
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

VS_OUT VS_OUTLINE(VS_IN In)
{
    VS_OUT Out;
    
    float3 vNormal = normalize(In.vPosition);
    float3 vExpandedPosition = In.vPosition + vNormal * g_fOutlineWidth;
    
    /* In.vPosition * 월드 * 뷰 * 투영 */    
    //float4x4 == matrix
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(vExpandedPosition, 1.f), matWVP);
    
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
    float3 vTexcoord : TEXCOORD0;
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
    
    return Out;   
}

PS_OUT PS_PAD(PS_IN In)
{
    PS_OUT Out;
    
    float4 vBlack = float4(0.f, 0.f, 0.f, 1.f);
    float4 vPink = float4(1.f, 0.75f, 0.8f, 1.f);
    
    // lerp(시작, 끝, 가중치)
    Out.vColor = lerp(vBlack, vPink, g_fColorWeight);
    
    return Out;   
}

PS_OUT PS_OUTLINE(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
    
    return Out;   
}

PS_OUT PS_MAIN_EDGE(PS_IN In)
{
    PS_OUT Out;

    // 1. 기본 면 색상 계산 (초록 ↔ 파랑 Lerp)
    float4 vGreen = float4(0.f, 1.f, 0.f, 1.f);
    float4 vBlue = float4(0.f, 0.f, 1.f, 1.f);
    float4 vBaseColor = lerp(vGreen, vBlue, g_fColorWeight);

    float3 vAbsPos = abs(In.vTexcoord);
    
    float fEdgeWidth = 0.45f; // 이 값을 낮추면 테두리가 두꺼워집니다.
    
    int iEdgeCount = 0;
    if (vAbsPos.x > fEdgeWidth)
        iEdgeCount++;
    if (vAbsPos.y > fEdgeWidth)
        iEdgeCount++;
    if (vAbsPos.z > fEdgeWidth)
        iEdgeCount++;

    if (iEdgeCount >= 2)
    {
        Out.vColor = float4(1.f, 1.f, 1.f, 1.f); 
    }
    else
    {
        Out.vColor = vBaseColor; 
    }

    return Out;
}

technique11 DefaultTechnique
{ 
    pass Sky
    {   
        SetRasterizerState(RS_Cull_Front);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();        
    }

    pass Pad
    {   
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_PAD();
    }

    pass Outline
    {   
        SetRasterizerState(RS_Cull_Front);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_OUTLINE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_OUTLINE();
    }

    pass FrameEdge
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EDGE();
    }
}