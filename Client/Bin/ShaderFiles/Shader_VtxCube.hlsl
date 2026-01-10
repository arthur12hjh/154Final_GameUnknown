#include "Client_Shader_Utils.hlsli"

/* 깊이 렌더타겟 마스킹용 */
bool g_IsMaskingDepthB;
bool g_IsMaskingDepthW;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
textureCUBE g_Texture;
texture2D g_MaskTexture;
float g_fWeight0, g_fWeight1, g_fWeight2;
float g_fFar;
float g_fTime;
float g_fBeatRandom;  // 비트인디케이터 올라가는 막대 사이즈 랜덤값
float g_fColorWeight;
float g_fOutlineWidth = 0.02f;

 // 1. 가장 정석적인 쨍한 네온 스카이블루
float4 vNeonSkyBlue = float4(0.0f, 0.7f, 1.0f, 1.0f);
    // 2. 약간 더 민트/에메랄드 빛이 섞인 시원한 사이버펑크 블루
float4 vNeonCyan = float4(0.0f, 1.0f, 1.0f, 1.0f);
    // 3. 발광했을 때 중심이 아주 밝아 보이는 연한 네온 블루
float4 vSoftNeonBlue = float4(0.4f, 0.8f, 1.0f, 1.0f);
float4 vNeonPink = float4(1.0f, 0.05f, 0.6f, 1.0f);

bool g_bIsIdx0, g_bIsIdx1, g_bIsIdx2;

int g_iAccuracy = -1;

/* 정점 쉐이더 : */
/* 정점에 대한 셰이딩 == 정점에 필요한 연산을 수행한다 == 정점의 상태변환(월드, 뷰, 투영) + 추가변환 */
/* 정점의 구성 정보를 수정, 변경한다 */ 
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
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
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), matWVP)).xyz;
    /* Out.vPosition.xy => 시야각에 있는 점들을 90에 맞춰준다 */ 
    /* Out.vPosition.z => n~f사이에 있는 점들의 z를 0 ~ f로 바꿔준다. */  
    Out.vPosition.z -= 0.00001f;
    Out.vTexcoord = In.vTexcoord;    
    Out.vProjPos = Out.vPosition;
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
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), matWVP)).xyz;
    
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
    float3 vNormal : NORMAL;
    float3 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vORM : SV_Target3;
    float4 vEmissive : SV_TARGET4;
    float4 vBloom : SV_TARGET5;
};

float Random(float2 st)
{
    return frac(sin(dot(st.xy, float2(12.9898, 78.233))) * 43758.5453123);
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vNormal = float4(In.vNormal, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    Out.vORM = float4(0.f, 0.f, 0.f, 0.f);
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    return Out;   
}

PS_OUT PS_RAIL(PS_IN In)
{
    PS_OUT Out;
    float4 vBlack = float4(0.f, 0.f, 0.f, 1.f);
    float4 vWhite = float4(1.f, 1.f, 1.f, 1.f);
    
    float4 vSkyBlue = float4(0.4f, 0.9f, 1.0f, 1.0f);
    float4 vPerfect = float4(1.00f, 0.35f, 0.80f, 1.0f);
    float4 vGood = float4(0.55f, 1.00f, 0.65f, 1.0f);
    float4 vMiss = float4(1.00f, 0.20f, 0.20f, 1.0f);
    float4 vBad = float4(0.95f, 0.95f, 1.00f, 1.0f);
    float4 vAccuracy = 0.f;
    
    // 모든 칸 색상을 밝은 연두로 통일
    float4 vBrightLime = float4(0.5f, 1.0f, 0.0f, 1.0f);

    float4 vColors[5];
    //vColors[0] = float4(0.30f, 0.70f, 0.00f, 1.0f); // 진한 연두 (Start)
    //vColors[1] = float4(0.42f, 0.77f, 0.12f, 1.0f);
    //vColors[2] = float4(0.55f, 0.85f, 0.25f, 1.0f); // 중간 단계
    //vColors[3] = float4(0.67f, 0.92f, 0.37f, 1.0f);
    //vColors[4] = float4(0.80f, 1.00f, 0.50f, 1.0f); // 밝은 연두 (End)
    
    //vColors[0] = float4(0.00f, 0.30f, 0.70f, 1.0f); // 진한 연두 (Start)
    //vColors[1] = float4(0.06f, 0.36f, 0.74f, 1.0f);
    //vColors[2] = float4(0.12f, 0.42f, 0.78f, 1.0f); 
    //vColors[3] = float4(0.18f, 0.48f, 0.82f, 1.0f); 
    //vColors[4] = float4(0.25f, 0.55f, 0.85f, 1.0f); // 중간 단계
    
    vColors[0] = float4(0.2f, 0.30f, 0.70f, 1.0f); // 딥 블루 (Start)
    vColors[1] = float4(0.4f, 0.32f, 0.75f, 1.0f); // 블루 + 퍼플 기미
    vColors[2] = float4(0.6f, 0.28f, 0.75f, 1.0f); // 퍼플
    vColors[3] = float4(0.8f, 0.30f, 0.65f, 1.0f); // 퍼플 → 핑크
    vColors[4] = float4(1.00f, 0.35f, 0.55f, 1.0f); // 핑크 (End)
    
    float fXPos = (In.vTexcoord.x + 0.5f);
    int iIdx = (int) floor(fXPos * 3.0f);
    iIdx = clamp(iIdx, 0, 2);

    float fYPos = (In.vTexcoord.y + 0.5f);
    float fColorIdx = fYPos * 4.0f;
    int iColorIdx = (int) floor(fColorIdx);
    float fWeight = frac(fColorIdx);

    float4 vTargetColor = lerp(vColors[clamp(iColorIdx, 0, 4)], vColors[clamp(iColorIdx + 1, 0, 4)], smoothstep(0.0f, 1.0f, fWeight));
    
    // 현재 픽셀이 속한 칸이 활성화되었는지 확인
    bool bIsActive = false;
    float fMyWeight = 0.f;
    if (iIdx == 0)
    {
        bIsActive = g_bIsIdx0;
        fMyWeight = g_fWeight0;
    }
    else if (iIdx == 1)
    {
        bIsActive = g_bIsIdx1;
        fMyWeight = g_fWeight1;
    }
    else if (iIdx == 2)
    {
        bIsActive = g_bIsIdx2;
        fMyWeight = g_fWeight2;
    }

    if(g_iAccuracy == -1)
        vAccuracy = vSkyBlue;
    else if(g_iAccuracy == 0)
        vAccuracy = vMiss;
    else if(g_iAccuracy == 1)
        vAccuracy = vBad;
    else if(g_iAccuracy == 2)
        vAccuracy = vGood;
    else if(g_iAccuracy == 3)
        vAccuracy = vPerfect;
    
    float fThreshold = lerp(-0.49f, 0.4f, fMyWeight);
    float fHeightMask = 1.f - smoothstep(fThreshold - 0.1f, fThreshold + 0.1f, In.vTexcoord.y);

    float4 vBaseColor = vBlack;
    if (fMyWeight > 0.0001f)
    {
        //vBaseColor = lerp(vBlack, vBrightLime, fHeightMask);
        vBaseColor = lerp(vBlack, vTargetColor, fHeightMask);
    }
    
    float3 vAbsPos = abs(In.vTexcoord);
    
    float fEdgeWidth = 0.45f;
    float fCurrentPos = vAbsPos.x;

    if (vAbsPos.x > 0.45f && vAbsPos.z > 0.45f)
    {
        float4 vGradColor;
    
        // 구간 1: 하늘
        if (vAbsPos.x <= 0.465f && vAbsPos.z <= 0.50f)
        {
            float fRatio = (vAbsPos.x - 0.45f) / (0.465f - 0.45f);
            vGradColor = lerp(vAccuracy, vWhite, fRatio);
        }
        // 구간 2: 흰색
        else if (vAbsPos.x <= 0.467f && vAbsPos.z <= 0.50f)
        {
            vGradColor = vWhite;
        }
        // 구간 3: 하늘
        else if (vAbsPos.x <= 0.50f && vAbsPos.z <= 0.5f)
        {
            float fRatio = (vAbsPos.x - 0.467f) / (0.50f - 0.467f);
            vGradColor = lerp(vWhite, vAccuracy, fRatio);
        }
        else
        {
            vGradColor = vBlack;
        }
    
        Out.vColor = vGradColor;
        Out.vBloom = vAccuracy * 2.f;
    }
    else
    {
        Out.vColor = vBaseColor;
        Out.vBloom = vBaseColor * 1.5f;
    }
    Out.vColor.a = 1.0f;

    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    Out.vORM = float4(0.f, 0.f, 0.f, 0.f);
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    //Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    return Out;
}

PS_OUT PS_OUTLINE(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    Out.vORM = float4(0.f, 0.f, 0.f, 0.f);
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    return Out;   
}

PS_OUT PS_MAIN_PAD(PS_IN In)
{
    PS_OUT Out;

    // 1. 기본 면 색상 계산 (초록 ↔ 파랑 Lerp)
    float4 vBlack = float4(0.f, 0.f, 0.f, 1.f);
    float4 vPink = float4(1.f, 0.75f, 0.8f, 1.f);
    float4 vWhite = float4(1.f, 1.f, 1.f, 1.f);
    float4 vSkyBlue = float4(0.4f, 0.9f, 1.0f, 1.0f); // 요청하신 하늘색
    float4 vBaseColor = lerp(vBlack, vPink, g_fColorWeight);

    float3 vAbsPos = abs(In.vTexcoord);
    
    float fEdgeWidth = 0.48f; 
    
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
        Out.vBloom = vNeonSkyBlue * 2.f;
    }
    else
    {
        Out.vColor = vBaseColor;
        Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    }
    
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    Out.vORM = float4(0.f, 0.f, 0.f, 0.f);
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    return Out;
}

PS_OUT PS_MAIN_BEAT_INDICATOR(PS_IN In)
{
    PS_OUT Out;

    float4 vBlack = float4(0.f, 0.f, 0.f, 1.f);
    float4 vWhite = float4(1.f, 1.f, 1.f, 1.f);
    float4 vSkyBlue = float4(0.4f, 0.9f, 1.0f, 1.0f);

    float4 vColors[10];
    vColors[0] = float4(0.2f, 0.0f, 0.2f, 1.0f); // 딥 퍼플 (가장 어두움)
    vColors[1] = float4(0.35f, 0.0f, 0.4f, 1.0f);
    vColors[2] = float4(0.5f, 0.0f, 0.6f, 1.0f);
    vColors[3] = float4(0.65f, 0.0f, 0.7f, 1.0f);
    vColors[4] = float4(0.8f, 0.1f, 0.6f, 1.0f); // 마젠타 계열
    vColors[5] = float4(1.0f, 0.2f, 0.5f, 1.0f); // 핫 핑크
    vColors[6] = float4(1.0f, 0.4f, 0.7f, 1.0f);
    vColors[7] = float4(1.0f, 0.6f, 0.9f, 1.0f); // 밝은 핑크
    vColors[8] = float4(0.9f, 0.7f, 1.0f, 1.0f); // 연보라
    vColors[9] = float4(1.0f, 0.9f, 1.0f, 1.0f); // 화이트 핑크 (가장 밝음)

    float fPos = (In.vTexcoord.y + 0.5f);
    float fColorIdx = fPos * 9.0f;
    int iIdx = (int) floor(fColorIdx);
    float fWeight = frac(fColorIdx);

    float4 vTargetColor = lerp(vColors[clamp(iIdx, 0, 9)], vColors[clamp(iIdx + 1, 0, 9)], smoothstep(0.0f, 1.0f, fWeight));

    float fSmoothWeight = lerp(0.f, 1.f, g_fColorWeight);
    
    float fThreshold = lerp(-0.4f, g_fBeatRandom, fSmoothWeight);
    float fHeight = 1.f - smoothstep(fThreshold - 0.1f, fThreshold + 0.1f, In.vTexcoord.y);

    float4 vBaseColor = lerp(vBlack, vTargetColor, fHeight);
    
    float3 vAbsPos = abs(In.vTexcoord);
    float fEdgeWidth = 0.48f;
    
    int iEdgeCount = 0;
    if (vAbsPos.x > fEdgeWidth)
        iEdgeCount++;
    if (vAbsPos.y > fEdgeWidth)
        iEdgeCount++;
    if (vAbsPos.z > fEdgeWidth)
        iEdgeCount++;

    if (iEdgeCount >= 2)
    {
        float fDistX = abs(vAbsPos.x - 0.485f);
        float fDistY = abs(vAbsPos.y - 0.485f);
        float fDistZ = abs(vAbsPos.z - 0.485f);

        float fMinDist = 1.0f;
        if (vAbsPos.x > fEdgeWidth)
            fMinDist = min(fMinDist, fDistX);
        if (vAbsPos.y > fEdgeWidth)
            fMinDist = min(fMinDist, fDistY);
        if (vAbsPos.z > fEdgeWidth)
            fMinDist = min(fMinDist, fDistZ);

        float fFinalMask = 1.0f - smoothstep(0.0f, 0.015f, fMinDist);
        Out.vColor = lerp(vSkyBlue, vWhite, fFinalMask);
        Out.vColor.a = 1.0f;
        Out.vBloom = vNeonPink * 2.f;
        
    }
    else
    {
        Out.vColor = vBaseColor;
        Out.vBloom = vBaseColor * 1.5f;
    }
    
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    Out.vORM = float4(0.f, 0.f, 0.f, 0.f);
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    return Out;
}

PS_OUT PS_MAIN_BOX(PS_IN In)
{
    PS_OUT Out;

    float4 vBlack = float4(0.f, 0.f, 0.f, 1.f);
    float4 vPink = float4(1.f, 0.75f, 0.8f, 1.f);
    float4 vWhite = float4(1.f, 1.f, 1.f, 1.f);
    float4 vNeonPurple = float4(0.5f, 0.0f, 1.0f, 1.0f);
    float4 vNeonWhite = float4(1.0f, 0.9f, 1.0f, 1.0f);
    float4 vBaseColor = vBlack;

    float3 vLocalNormal = normalize(In.vNormal);
    bool bIsFrontFace = vLocalNormal.z < -0.8f;

    
    float4 vFinalColor = vBaseColor;
    float2 vUV = In.vTexcoord.xy + 0.5f;
    float4 vMask = g_MaskTexture.Sample(DefaultSampler, vUV);

    float fGradientIdx = In.vTexcoord.y + 0.5f;
    float4 vGradientColor = lerp(vNeonPurple, vNeonPink, fGradientIdx);
    
    if (vMask.r > 0.1f)
    {
        vFinalColor = vGradientColor * 15.f;
    }

    float3 vAbsPos = abs(In.vTexcoord);
    
    float fEdgeWidth = 0.48f;
    
    int iEdgeCount = 0;
    if (vAbsPos.x > fEdgeWidth)
        iEdgeCount++;
    if (vAbsPos.y > fEdgeWidth)
        iEdgeCount++;
    if (vAbsPos.z > fEdgeWidth)
        iEdgeCount++;
    
    if (iEdgeCount >= 2)
    {
        Out.vColor = vNeonWhite;
        Out.vBloom = vNeonPink * 5.f;
    }
    else
    {
        Out.vColor = vFinalColor;
        if (vMask.r > 0.1f)
            Out.vBloom = vFinalColor * 0.2f;
        else
            Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    }
    
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    Out.vORM = float4(0.f, 0.f, 0.f, 0.f);
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    
    
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
    
    // idx 1
    pass Rail
    {   
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RAIL();
    }
    
    // idx 2
    pass Outline
    {   
        SetRasterizerState(RS_Cull_Front);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_OUTLINE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_OUTLINE();
    }

    // idx 3
    pass Pad
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PAD();
    }
    
    // idx 4
    pass BeatIndicator
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BEAT_INDICATOR();
    }

    // idx 5
    pass Box
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BOX();
    }
}