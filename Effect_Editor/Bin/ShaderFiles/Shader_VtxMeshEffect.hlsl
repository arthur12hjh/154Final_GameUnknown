
#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vCamPosition;
vector g_vColor = vector(1.f, 1.f, 1.f, 1.f);
vector g_vSize = vector(1.f, 0.f, 1.f, 0.f);
float g_fTime = 0;
float2 g_fMaskUV = float2(0, 0);
float2 g_fMaskUVSpeed = float2(0, 0);
float2 g_fMaskUVSize = float2(1, 1);
float2 g_fDiffuseUV = float2(0, 0);
float2 g_fDiffuseUVSpeed = float2(0, 0);
float2 g_fDiffuseUVSize = float2(1, 1);
float2 g_fDissolveUV = float2(0, 0);
float2 g_fDissolveUVSpeed = float2(0, 0);
float2 g_fDissolveUVSize = float2(1, 1);

texture2D g_MaskTexture, g_DiffuseTexture, g_DissolveTexture;
/* 정점 쉐이더 : */
/* 정점에 대한 셰이딩 == 정점에 필요한 연산을 수행한다 == 정점의 상태변환(월드, 뷰, 투영) + 추가변환 */
/* 정점의 구성 정보를 수정, 변경한다 */ 
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(mul(vector(In.vBinormal, 0.f), g_WorldMatrix)).xyz;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    return Out;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};


VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;

    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
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
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vRimLight : SV_TARGET3;
};



/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    
    //Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r;
    //Out.vDiffuse.a *= ((fireFront.r + fireBack.r) / 2) * ((In.vLifeTime.y - In.vLifeTime.x) / In.vLifeTime.y);
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    if (In.vTexcoord.y < 0.5)
    {
        Out.vDiffuse = g_vColor;
        Out.vDiffuse *= (1 - abs(MaskTexcoord.x)) * abs(MaskTexcoord.y) * 2;
        Out.vDiffuse *= g_DiffuseTexture.Sample(DefaultSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y * 2));
        Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x, MaskTexcoord.y * 2)).r;
        if (g_DissolveTexture.Sample(DefaultSampler, float2(DissolveTexcoord.x, DissolveTexcoord.y * 2)).r > (1 - abs(MaskTexcoord.x)) * abs(MaskTexcoord.y) * 2 * 2)
            discard;

    }
    else
    {
        Out.vDiffuse = g_vColor;
        Out.vDiffuse *= (1 - MaskTexcoord.x) * (1 - (MaskTexcoord.y - 0.5) * 2);
        Out.vDiffuse *= g_DiffuseTexture.Sample(DefaultSampler, float2(1 - DiffuseTexcoord.x, 1 - (DiffuseTexcoord.y - 0.5) * 2));
        Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x, 1 - (MaskTexcoord.y - 0.5) * 2)).r;
        if (g_DissolveTexture.Sample(DefaultSampler, float2(DissolveTexcoord.x, 1 - (DissolveTexcoord.y - 0.5) * 2)).r > (1 - MaskTexcoord.x) * (1 - (MaskTexcoord.y - 0.5) * 2) * 2)
            discard;
    }
    //
    //if (Out.vDiffuse.a <= 0.1f || 0 > fmod(g_fTime, 1))
    //    discard;
    //Out.vNormal = float4(In.vNormal * 0.5f + 0.5f, 1.f);
    //Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 1.0f);
    return Out;
}
/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_DISTORTION(PS_IN In)
{
    PS_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    if (0 > MaskTexcoord.x || 1 < MaskTexcoord.x)
        discard;
    if (In.vTexcoord.y < 0.5)
    {
        Out.vDiffuse = g_vColor;
        Out.vDiffuse *= (1 - abs(MaskTexcoord.x)) * abs(MaskTexcoord.y) * 2;
        //Out.vDiffuse.a = g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x, MaskTexcoord.y * 2)).r;

    }
    else
    {
        Out.vDiffuse = g_vColor;
        Out.vDiffuse *= (1 - MaskTexcoord.x) * (1 - (MaskTexcoord.y - 0.5) * 2);
        //Out.vDiffuse.a = g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x, 1 - (MaskTexcoord.y - 0.5) * 2)).r;
    }
    return Out;
}

struct PS_IN_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

struct PS_OUT_SHADOW
{
    float4 vShadowLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    
    Out.vShadowLightDepth.x = In.vProjPos.w / 500.0f;;
    
    return Out;
}

technique11 DefaultTechnique
{ 
    pass UI
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Distortion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }


 
    

 
}