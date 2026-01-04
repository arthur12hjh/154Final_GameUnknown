
#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_CamMatrix;
vector g_vColor = vector(1.f, 1.f, 1.f, 1.f);
vector g_vSize = vector(1.f, 0.f, 1.f, 0.f);
float g_fTime = 0;
float g_fEndTime = 0;
float2 g_fMaskUV = float2(0, 0);
float2 g_fMaskUVSpeed = float2(0, 0);
float2 g_fMaskUVSize = float2(1, 1);
float2 g_fDiffuseUV = float2(0, 0);
float2 g_fDiffuseUVSpeed = float2(0, 0);
float2 g_fDiffuseUVSize = float2(1, 1);
float2 g_fDissolveUV = float2(0, 0);
float2 g_fDissolveUVSpeed = float2(0, 0);
float2 g_fDissolveUVSize = float2(1, 1);
float g_fFar;

texture2D g_MaskTexture, g_DiffuseTexture, g_DissolveTexture;
texture2D g_DepthTexture;
int g_iSizeCount;
StructuredBuffer<float3> g_fSizeDiagram : register(t0);




sampler YMirrorSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = mirror;
};

sampler YNoneSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = border;
    BorderColor = float4(0, 0, 0, 0);
};

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
    if (0 < g_fEndTime)
    {
        float time = g_fTime / g_fEndTime;
        float3 fInTime = float3(0, 0, 0);
        float3 fOutTime = float3(-1, 0, 0);
        float3 vPosition = In.vPosition;
        for (int i = 0; i < g_iSizeCount; ++i)
        {
            if (g_fSizeDiagram[i].x <= time)
            {
                fInTime.x = g_fSizeDiagram[i].x;
                fInTime.y = g_fSizeDiagram[i].y;
                fInTime.z = g_fSizeDiagram[i].z;
            }
            if (g_fSizeDiagram[i].x > time)
            {
                fOutTime.x = g_fSizeDiagram[i].x;
                fOutTime.y = g_fSizeDiagram[i].y;
                fOutTime.z = g_fSizeDiagram[i].z;
                break;
            }
        }
        if (-1 == fOutTime.x)
            vPosition.xyz *= fInTime.y;
        else
        {
            float t = (time - fInTime.x) / (fOutTime.x - fInTime.x);
            float fSize = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * fInTime.y
                        + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(fInTime.z)) * (fOutTime.x - fInTime.x) * 100
                        + (-2 * pow(t, 3) + 3 * pow(t, 2)) * fOutTime.y
                        + (pow(t, 3) - pow(t, 2)) * tan(radians(fOutTime.z)) * (fOutTime.x - fInTime.x) * 100;
            vPosition.xyz *= fSize;
        }

        Out.vPosition = mul(vector(vPosition, 1.f), matWVP);
        Out.vTexcoord = In.vTexcoord;
        Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
        Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
        Out.vBinormal = normalize(mul(vector(In.vBinormal, 0.f), g_WorldMatrix)).xyz;
        Out.vWorldPos = mul(vector(vPosition, 1.f), g_WorldMatrix);
        Out.vProjPos = Out.vPosition;
    }
    else
    {
        Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
        Out.vTexcoord = In.vTexcoord;
        Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
        Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
        Out.vBinormal = normalize(mul(vector(In.vBinormal, 0.f), g_WorldMatrix)).xyz;
        Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
        Out.vProjPos = Out.vPosition;
    }
    return Out;
}

VS_OUT VS_CONE(VS_IN In)
{
    VS_OUT Out;
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    float3 vPosition = In.vPosition;
    vPosition.y = length(In.vPosition) * 1.5;
    Out.vPosition = mul(vector(vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(mul(vector(In.vBinormal, 0.f), g_WorldMatrix)).xyz;
    Out.vWorldPos = mul(vector(vPosition, 1.f), g_WorldMatrix);
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



struct PS_NONBLEND_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vORM : SV_Target3;
    float4 vEmissive : SV_TARGET4;
    float4 vBloom : SV_TARGET5;
};

struct PS_NORMAL_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

struct PS_NONLIGHT_OUT
{
    float4 vColor : SV_TARGET0;
};

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONBLEND_OUT PS_MAIN(PS_IN In)
{
    PS_NONBLEND_OUT Out;
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    if (0 > MaskTexcoord.y || 1 < MaskTexcoord.y)
        discard;
    Out.vDiffuse = g_vColor;
    Out.vNormal = float4(normalize(In.vNormal) * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 1.0f);
    Out.vORM = float4(0.f, 0.f, 0.f, 0.f);
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_DISTORTION(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    float2 MaskTexcoord = 1 - float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    if (0 > MaskTexcoord.x || 1 < MaskTexcoord.x || 0 > MaskTexcoord.y || 1 < MaskTexcoord.y)
        discard;
    Out.vColor = g_vColor;
    Out.vColor *= (1 - abs(MaskTexcoord.x)) * abs(MaskTexcoord.y) * 2;
    if (0 >= Out.vColor.a)
        discard;
    return Out;
}
/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_CIRCLE_DISTORTION(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    if (0 > MaskTexcoord.y || 1 < MaskTexcoord.y)
        discard;
    Out.vColor = g_vColor;
    Out.vColor *= abs(MaskTexcoord.y) * 2;
    if (0 >= Out.vColor.a)
        discard;
    return Out;
}


/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_SLASH(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = 1 - float2((In.vTexcoord.x + g_fMaskUV.x + min(g_fTime * g_fMaskUVSpeed.x, 1)) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    Out.vColor = g_vColor;
    Out.vColor.rgb *= g_DiffuseTexture.Sample(MirrorSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y));
    Out.vColor.a *= (1 - abs(MaskTexcoord.x));
    Out.vColor.a *= g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x, MaskTexcoord.y)).r;
    //if (g_DissolveTexture.Sample(MirrorSampler, float2(DissolveTexcoord.x, DissolveTexcoord.y)).r + (1 - abs(MaskTexcoord.x * 1.1)) < abs(MaskTexcoord.x) + g_fTime * 2)
    //    discard;
    
    if (0 >= Out.vColor.a)
        discard;
    return Out;
}
// * max((abs(1 - saturate(g_fTime * 3))) * 4, 0.8)
/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_SLASH_BLOOM(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = 1 - float2((In.vTexcoord.x + g_fMaskUV.x + min(g_fTime * g_fMaskUVSpeed.x, 1)) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    Out.vColor = g_vColor;
    Out.vColor.rgb *= g_DiffuseTexture.Sample(MirrorSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y));
    Out.vColor.a *= (1 - abs(MaskTexcoord.x));
    Out.vColor.a *= g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x, MaskTexcoord.y)).r;
    if (0 >= Out.vColor.a)
        discard;
    Out.vColor.rgb = saturate(pow(Out.vColor.rgb * 10, 2));
    Out.vColor.rgb *= Out.vColor.a;
    Out.vColor.a = 1;
    return Out;
}


float Random(float fSeed, float fMin, float fMax)
{
    return lerp(fMin, fMax, frac(sin(fSeed) * 13524.5313));
}

PS_NONLIGHT_OUT PS_LIGHTNING(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + min(g_fTime * g_fMaskUVSpeed.x, 1)) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    if (((In.vTexcoord.x + g_fMaskUV.x + min(g_fTime * g_fMaskUVSpeed.x, 1)) * g_fMaskUVSize.x) * g_fDissolveUVSize.x < 0)
        discard;
    Out.vColor = g_vColor;
    Out.vColor.a *= g_MaskTexture.Sample(NoneSampler, MaskTexcoord).r;
    
    Out.vColor *= pow(saturate((g_fTime) / (In.vTexcoord.x)), 2) * pow(saturate((In.vTexcoord.x) / (g_fTime)), 5);
    
    if (0 >= Out.vColor.a)
        discard;
    return Out;
}

PS_NONLIGHT_OUT PS_LIGHTNING_BLOOM(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + min(g_fTime * g_fMaskUVSpeed.x, 1)) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    if (((In.vTexcoord.x + g_fMaskUV.x + min(g_fTime * g_fMaskUVSpeed.x, 1)) * g_fMaskUVSize.x) * g_fDissolveUVSize.x < 0)
        discard;
    Out.vColor = g_vColor;
    Out.vColor.a *= g_MaskTexture.Sample(NoneSampler, MaskTexcoord).r;
    
    Out.vColor *= pow(saturate((g_fTime) / (In.vTexcoord.x)), 2) * pow(saturate((In.vTexcoord.x) / (g_fTime)), 5);
    if (0 >= Out.vColor.a)
        discard;
    Out.vColor.rgb = saturate(pow(Out.vColor.rgb * 10, 2));
    Out.vColor.rgb *= Out.vColor.a;
    Out.vColor.a = 1;
    return Out;
}
/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_HIT(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    if (0 > MaskTexcoord.y || 1 < MaskTexcoord.y)
        discard;
    
    Out.vColor = g_vColor;
    vector color = g_DiffuseTexture.Sample(MirrorSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y)) * g_vColor;
    Out.vColor.rgb = Out.vColor.rgb * (1 - color.a) + color.rgb * color.a;
    Out.vColor.a = color.a * g_MaskTexture.Sample(DefaultSampler, float2(MaskTexcoord.x, MaskTexcoord.y)).r * (In.vTexcoord.y >= 0.8 ? 1 - (In.vTexcoord.y - 0.8) * 5 : 1);
    if (g_DissolveTexture.Sample(MirrorSampler, float2(DissolveTexcoord.x, DissolveTexcoord.y)).r > abs(MaskTexcoord.y))
        discard;
    if (0.5 >= Out.vColor.a)
        discard;
    Out.vColor.a = saturate(Out.vColor.a * 3);
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_HIT_BLOOM(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    if (0 > MaskTexcoord.y || 1 < MaskTexcoord.y)
        discard;
    
    Out.vColor = g_vColor;
    vector color = g_DiffuseTexture.Sample(MirrorSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y)) * g_vColor;
    //color.a = (1 - abs(MaskTexcoord.x * 1.2));
    Out.vColor.rgb = Out.vColor.rgb * (1 - color.a) + color.rgb * color.a;
    Out.vColor.a = color.a * g_MaskTexture.Sample(DefaultSampler, float2(MaskTexcoord.x, MaskTexcoord.y)).r * (In.vTexcoord.y >= 0.8 ? 1 - (In.vTexcoord.y - 0.8) * 5 : 1);
    if (g_DissolveTexture.Sample(MirrorSampler, float2(DissolveTexcoord.x, DissolveTexcoord.y)).r > abs(MaskTexcoord.y))
        discard;
    if (0 >= Out.vColor.a)
        discard;
    Out.vColor.rgb *= Out.vColor.a;
    Out.vColor.a = 1;
    
    return Out;
}

PS_NONLIGHT_OUT PS_SLASH_LOOP(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    Out.vColor = g_vColor / 6;
    if (0.5 > In.vTexcoord.y)
    {
        Out.vColor.rgb *= g_DiffuseTexture.Sample(DefaultSampler, DiffuseTexcoord);
        Out.vColor.a *= g_MaskTexture.Sample(DefaultSampler, MaskTexcoord).r;
    }
    else
    {
        Out.vColor.rgb *= g_DiffuseTexture.Sample(DefaultSampler, float2(DiffuseTexcoord.x, 1 - DiffuseTexcoord.y));
        Out.vColor.a *= g_MaskTexture.Sample(DefaultSampler, float2(MaskTexcoord.x, 1 - MaskTexcoord.y)).r;
    }
    if (0 >= Out.vColor.a)
        discard;
    return Out;
}

PS_NONLIGHT_OUT PS_SLASH_LOOP_BLOOM(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    Out.vColor = g_vColor;
    if (0.5 > In.vTexcoord.y)
    {
        Out.vColor.rgb *= g_DiffuseTexture.Sample(DefaultSampler, DiffuseTexcoord);
        Out.vColor.a *= g_MaskTexture.Sample(DefaultSampler, MaskTexcoord).r;
    }
    else
    {
        Out.vColor.rgb *= g_DiffuseTexture.Sample(DefaultSampler, float2(1 - DiffuseTexcoord.x, 1 - DiffuseTexcoord.y));
        Out.vColor.a *= g_MaskTexture.Sample(DefaultSampler, float2(1 - MaskTexcoord.x, 1 - MaskTexcoord.y)).r;
    }
    if (0 >= Out.vColor.a)
        discard;
    Out.vColor.rgb *= Out.vColor.a;
    Out.vColor.a = 1;
    return Out;
}

PS_NONLIGHT_OUT PS_CIRCLE_SLASH(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    float fSize = lerp(1, 2, abs((1 + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x - 1));
    float4 vColor = g_vColor;
    if (1 > g_fTime * g_fMaskUVSpeed.x)
        MaskTexcoord.x = (In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x * fSize;
    else
    {
        MaskTexcoord.x = (In.vTexcoord.x + g_fMaskUV.x + 1) * g_fMaskUVSize.x * (fSize * g_fTime * g_fMaskUVSpeed.x);
        vColor.a *= 2 - pow(g_fTime * g_fMaskUVSpeed.x, 1.5);
    }
    Out.vColor = vColor;
    if (0.5 > In.vTexcoord.y)
    {
        //Out.vColor.rgb *= g_DiffuseTexture.Sample(ClampSampler, DiffuseTexcoord);
        MaskTexcoord.y = (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y;
    }
    else
    {
        MaskTexcoord.y = ((1 - In.vTexcoord.y) + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y;
    }
    Out.vColor.a *= g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x, MaskTexcoord.y * 2)).r * 1.5;
    if (0 >= Out.vColor.a)
        discard;
    return Out;
}

PS_NONLIGHT_OUT PS_CIRCLE_SLASH_BLOOM(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    float fSize = pow(lerp(1, 2, abs((1 + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x - 1)), 2);
    MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + min(g_fTime * g_fMaskUVSpeed.x, 1)) * g_fMaskUVSize.x * fSize, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    
    float4 vColor = g_vColor;
    if (1 > g_fTime * g_fMaskUVSpeed.x)
        MaskTexcoord.x = (In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x * fSize;
    else
    {
        MaskTexcoord.x = (In.vTexcoord.x + g_fMaskUV.x + 1) * g_fMaskUVSize.x * (fSize * g_fTime * g_fMaskUVSpeed.x);
        vColor.a *= 2 - pow(g_fTime * g_fMaskUVSpeed.x, 1.5);
    }
    Out.vColor = vColor;
    if (0.5 > In.vTexcoord.y)
    {
        //Out.vColor.rgb *= g_DiffuseTexture.Sample(ClampSampler, DiffuseTexcoord);
        MaskTexcoord.y = (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y;
    }
    else
    {
        MaskTexcoord.y = ((1 - In.vTexcoord.y) + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y;
    }
    Out.vColor.a *= g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x, MaskTexcoord.y * 2)).r * 1.5;
    if (0 >= Out.vColor.a)
        discard;
    Out.vColor.rgb *= Out.vColor.a;
    Out.vColor.a = 1;
    return Out;
}

PS_NONLIGHT_OUT PS_CIRCLE_SLASH_DIFFUSE(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    float fSize = lerp(1, 2, abs((1 + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x - 1));
    float4 vColor = g_vColor;
    if (1 > g_fTime * g_fMaskUVSpeed.x)
    {
        MaskTexcoord.x = (In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x * fSize;
        DiffuseTexcoord.x = (In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x * fSize;
    }
    else
    {
        MaskTexcoord.x = (In.vTexcoord.x + g_fMaskUV.x + 1) * g_fMaskUVSize.x * (fSize * g_fTime * g_fMaskUVSpeed.x);
        DiffuseTexcoord.x = (In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x * fSize;
        vColor.a *= 2 - pow(g_fTime * g_fMaskUVSpeed.x, 1.5);
    }
    Out.vColor = vColor;
    if (0.5 > In.vTexcoord.y)
    {
        //Out.vColor.rgb *= g_DiffuseTexture.Sample(ClampSampler, DiffuseTexcoord);
        MaskTexcoord.y = (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y;
        DiffuseTexcoord.y = (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y;
    }
    else
    {
        MaskTexcoord.y = ((1 - In.vTexcoord.y) + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y;
        DiffuseTexcoord.y = ((1 - In.vTexcoord.y) + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y;
    }
    Out.vColor.rgb *= max(g_DiffuseTexture.Sample(DefaultSampler, float2(MaskTexcoord.x, MaskTexcoord.y * 2)), 0.2);
    Out.vColor.a *= g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x, MaskTexcoord.y * 2)).r * 1.5;
    Out.vColor.a *= saturate(g_fTime * 2);
    if (0 >= Out.vColor.a)
        discard;
    return Out;
}

PS_NONLIGHT_OUT PS_CIRCLE_SLASH_DIFFUSE_BLOOM(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord;
    float2 DiffuseTexcoord;
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    float fSize = pow(lerp(1, 2, abs((1 + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x - 1)), 2);
    
    float4 vColor = g_vColor;
    if (1 > g_fTime * g_fMaskUVSpeed.x)
    {
        MaskTexcoord.x = (In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x * fSize;
        DiffuseTexcoord.x = (In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x * fSize;
    }
    else
    {
        MaskTexcoord.x = (In.vTexcoord.x + g_fMaskUV.x + 1) * g_fMaskUVSize.x * (fSize * g_fTime * g_fMaskUVSpeed.x);
        DiffuseTexcoord.x = (In.vTexcoord.x + g_fDiffuseUV.x + 1) * g_fDiffuseUVSpeed.x * (fSize * g_fTime * g_fDiffuseUVSpeed.x);
        vColor.a *= 2 - pow(g_fTime * g_fMaskUVSpeed.x, 1.5);
    }
    Out.vColor = vColor;
    if (0.5 > In.vTexcoord.y)
    {
        //Out.vColor.rgb *= g_DiffuseTexture.Sample(ClampSampler, DiffuseTexcoord);
        MaskTexcoord.y = (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y;
        DiffuseTexcoord.y = (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y;
    }
    else
    {
        MaskTexcoord.y = ((1 - In.vTexcoord.y) + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y;
        DiffuseTexcoord.y = ((1 - In.vTexcoord.y) + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y;
    }
    Out.vColor.rgb *= max(g_DiffuseTexture.Sample(DefaultSampler, float2(MaskTexcoord.x, MaskTexcoord.y * 2)), 0.2);
    Out.vColor.a *= g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x, MaskTexcoord.y * 2)).r * 10;
    Out.vColor.a *= saturate(g_fTime * 2);
    if (0 >= Out.vColor.a)
        discard;
    Out.vColor.rgb *= Out.vColor.a;
    Out.vColor.a = 1;
    return Out;
}

PS_NONLIGHT_OUT PS_CIRCLE_SLASH_NONLIGHT(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord;
    float2 DiffuseTexcoord;
    
    float4 vColor = g_vColor;
    if (1 > 1 + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x)
    {
        float fSize = lerp(3, 1, abs((1 + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x)));
        MaskTexcoord.x = (In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x * fSize;
        DiffuseTexcoord.x = (In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x * fSize;
    }
    else
    {
        float fSpeedTime = g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x;
        MaskTexcoord.x = ((In.vTexcoord.x + g_fMaskUV.x + (g_fTime * g_fMaskUVSpeed.x) * (1 - saturate(fSpeedTime * 0.25))) * g_fMaskUVSize.x) ;
        DiffuseTexcoord.x = ((In.vTexcoord.x + g_fDiffuseUV.x + 1 + (g_fTime * g_fDiffuseUVSpeed.x - 1)) * g_fDiffuseUVSize.x) % 1;
        vColor.a *= saturate((2 - pow(g_fTime * g_fMaskUVSpeed.x * 0.8, 1.5)));
    }
    if (0 > MaskTexcoord.x)
        discard;
    Out.vColor = vColor;
    if (0.5 > In.vTexcoord.y)
    {
        //Out.vColor.rgb *= g_DiffuseTexture.Sample(ClampSampler, DiffuseTexcoord);
        MaskTexcoord.y = (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y;
        DiffuseTexcoord.y = (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y;
    }
    else
    {
        MaskTexcoord.y = ((1 - In.vTexcoord.y) + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y;
        DiffuseTexcoord.y = ((1 - In.vTexcoord.y) + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y;
    }
    if (0 > MaskTexcoord.y)
        discard;
    //if (MaskTexcoord.x > 1 + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x || MaskTexcoord.x < g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x)
    //    discard;
    MaskTexcoord.x = fmod(MaskTexcoord.x, 1);
    Out.vColor.a *= max(g_DiffuseTexture.Sample(DefaultSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y * 2)).r, 0.2);
    Out.vColor.a *= g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x, MaskTexcoord.y * 2)).r;
    Out.vColor.a *= saturate(g_fTime * 2);
    if (0 >= Out.vColor.a)
        discard;
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = saturate(exp(-linearDepth * 20));
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a * weight * g_fDissolveUVSize.x;
    Out.vColor.a = Out.vColor.a * weight * g_fDissolveUVSize.x;
    return Out;
    //Out.vColor.rgb *= Out.vColor.a;
    //Out.vColor.a = 1;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_STING(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y) * g_fMaskUVSize.y);
    float2 MaskTexcoord2 = float2((In.vTexcoord.x + g_fMaskUV.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    MaskTexcoord.x += pow((1 - In.vTexcoord.y) * 1.2, 2.5);
    MaskTexcoord2.x += pow((1 - In.vTexcoord.y) * 1.2, 2.5);
    Out.vColor = g_vColor;
    //Out.vColor.rgb *= g_DiffuseTexture.Sample( DefaultSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y));
    
    Out.vColor.rgb *= Out.vColor.a * saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 3));
    Out.vColor.a *= saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 3));
    //Out.vColor.a *= saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 5));
    Out.vColor.a *= saturate(g_MaskTexture.Sample(DefaultSampler, MaskTexcoord).r * g_MaskTexture.Sample(YMirrorSampler, MaskTexcoord2).r * 2) * pow(g_DissolveTexture.Sample(NoneSampler, DissolveTexcoord).r * 2, 2);
    //if (g_DissolveTexture.Sample(MirrorSampler, float2(DissolveTexcoord.x, DissolveTexcoord.y)).r + (1 - abs(MaskTexcoord.x * 1.1)) < abs(MaskTexcoord.x) + g_fTime * 2)
    //    discard;
    
    if (0 >= Out.vColor.a)
        discard;
    return Out;
}
// * max((abs(1 - saturate(g_fTime * 3))) * 4, 0.8)
/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_STING_BLOOM(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y) * g_fMaskUVSize.y);
    float2 MaskTexcoord2 = float2((In.vTexcoord.x + g_fMaskUV.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    MaskTexcoord.x += pow((1 - In.vTexcoord.y) * 1.2, 2.5);
    MaskTexcoord2.x += pow((1 - In.vTexcoord.y) * 1.2, 2.5);
    Out.vColor = g_vColor;
    Out.vColor.rgb *= Out.vColor.a * saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 5));
    //Out.vColor.a *= saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 5));
    //Out.vColor.rgb *= g_DiffuseTexture.Sample(DefaultSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y));
    //Out.vColor.a *= saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 5));
    Out.vColor.a *= saturate(g_MaskTexture.Sample(DefaultSampler, MaskTexcoord).r * g_MaskTexture.Sample(YMirrorSampler, MaskTexcoord2).r * 2) * pow(g_DissolveTexture.Sample(NoneSampler, DissolveTexcoord).r * 2, 2);
    if (0 >= Out.vColor.a)
        discard;
    Out.vColor.rgb = saturate(pow(Out.vColor.rgb * 10, 2));
    Out.vColor.rgb *= Out.vColor.a;
    Out.vColor.a = 1;
    return Out;
}

PS_NONLIGHT_OUT PS_STING_SHOCK(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    float2 uv = In.vPosition.xy / float2(1600, 900);
    if (In.vProjPos.z / In.vProjPos.w > g_DepthTexture.Sample(DefaultSampler, uv * 2).r)
        discard;
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    float4 vColor = g_vColor;
    Out.vColor = vColor;
    if (0 > MaskTexcoord.y)
        discard;
    Out.vColor.a *= max(g_DiffuseTexture.Sample(DefaultSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y)).r, 0.2);
    Out.vColor.a *= g_MaskTexture.Sample(YNoneSampler, float2(MaskTexcoord.x, MaskTexcoord.y)).r;
    //Out.vColor.a *= saturate(g_fTime * 2);
    Out.vColor.a *= pow(In.vTexcoord.y * 5, 2);
    if (0 >= Out.vColor.a)
        discard;
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = saturate(exp(-linearDepth * 20));
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a * weight;
    Out.vColor.a = Out.vColor.a * weight;
    return Out;
}

PS_NONLIGHT_OUT PS_STING_SHOCK_DISTORTION(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    float4 vColor = g_vColor;
    Out.vColor = vColor;
    if (0 > MaskTexcoord.y)
        discard;
    Out.vColor.a *= max(g_DiffuseTexture.Sample(DefaultSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y)).r, 0.2);
    Out.vColor.a *= g_MaskTexture.Sample(YNoneSampler, float2(MaskTexcoord.x, MaskTexcoord.y)).r;
    //Out.vColor.a *= saturate(g_fTime * 2);
    Out.vColor.a *= pow(In.vTexcoord.y * 5, 2);
    if (0 >= Out.vColor.a)
        discard;
    
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    Out.vColor.a = Out.vColor.a;
    return Out;
}

PS_NONLIGHT_OUT PS_CIRCLE_SLASH_LOOP(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    float fSize = pow(lerp(1, 2, abs((1 + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) - 1)), 2);
    
    float4 vColor = g_vColor;
    vColor.a *= lerp(1, 0, (g_fTime * g_fMaskUVSpeed.x) * 0.4);
    if (1 > g_fTime * g_fMaskUVSpeed.x)
    {
        MaskTexcoord.x = (In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x * fSize;
        DiffuseTexcoord.x = (In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x * fSize;
    }
    else
    {
        float fSpeedTime = g_fTime * g_fMaskUVSpeed.x - 1;
        MaskTexcoord.x = ((In.vTexcoord.x + g_fMaskUV.x + 1 + (g_fTime * g_fMaskUVSpeed.x - 1) * (1 - saturate(fSpeedTime * 0.25)) * 0.65) * g_fMaskUVSize.x) % 1;
        DiffuseTexcoord.x = (In.vTexcoord.x + g_fDiffuseUV.x + 1) * g_fDiffuseUVSize.x * fSize;
        vColor.rgb = saturate(lerp(vColor.rgb, float3(1, 1, 1), (g_fTime * g_fMaskUVSpeed.x - 1) * 0.2));
    }
    Out.vColor = vColor;
    if (0.5 > In.vTexcoord.y)
    {
        //Out.vColor.rgb *= g_DiffuseTexture.Sample(ClampSampler, DiffuseTexcoord);
        MaskTexcoord.y = (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y;
        DiffuseTexcoord.y = (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y;
    }
    else
    {
        MaskTexcoord.y = ((1 - In.vTexcoord.y) + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y;
        DiffuseTexcoord.y = ((1 - In.vTexcoord.y) + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y;
    }
    //if (MaskTexcoord.x > g_fTime * g_fMaskUVSpeed.x || MaskTexcoord.x < g_fTime * g_fMaskUVSpeed.x - 1)
    //    discard;
    if (0 > MaskTexcoord.y || 1 < MaskTexcoord.y)
        discard;
    if (0 > MaskTexcoord.x || 1 < MaskTexcoord.x)
        discard;
    Out.vColor.rgb *= max(g_DiffuseTexture.Sample(DefaultSampler, float2(MaskTexcoord.x, MaskTexcoord.y * 2)), 0.2);
    Out.vColor.a *= g_MaskTexture.Sample(DefaultSampler, float2(MaskTexcoord.x, MaskTexcoord.y * 2)).r * 1.5;
    Out.vColor.a *= saturate(g_fTime * 2);
    if (0 >= Out.vColor.a)
        discard;
    return Out;
}

PS_NONLIGHT_OUT PS_CIRCLE_SLASH_LOOP_BLOOM(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord;
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    float fSize = pow(lerp(1, 2, abs((1 + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) - 1)), 2);
    
    MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * fSize, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float4 vColor = g_vColor;
    vColor.a *= lerp(1, 0, (g_fTime * g_fMaskUVSpeed.x) * 0.4);
    if (1 > g_fTime * g_fMaskUVSpeed.x)
    {
        MaskTexcoord.x = (In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x * fSize;
        DiffuseTexcoord.x = (In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x * fSize;
    }
    else
    {
        float fSpeedTime = g_fTime * g_fMaskUVSpeed.x - 1;
        MaskTexcoord.x = ((In.vTexcoord.x + g_fMaskUV.x + 1 + (g_fTime * g_fMaskUVSpeed.x - 1) * (1 - saturate(fSpeedTime * 0.25)) * 0.65) * g_fMaskUVSize.x) % 1;
        DiffuseTexcoord.x = (In.vTexcoord.x + g_fDiffuseUV.x + 1) * g_fDiffuseUVSize.x * fSize;
        vColor.rgb = saturate(lerp(vColor.rgb, float3(1, 1, 1), (g_fTime * g_fMaskUVSpeed.x - 1) * 0.2));
    }
    Out.vColor = vColor;
    if (0.5 > In.vTexcoord.y)
    {
        //Out.vColor.rgb *= g_DiffuseTexture.Sample(ClampSampler, DiffuseTexcoord);
        MaskTexcoord.y = (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y;
        DiffuseTexcoord.y = (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y;
    }
    else
    {
        MaskTexcoord.y = ((1 - In.vTexcoord.y) + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y;
        DiffuseTexcoord.y = ((1 - In.vTexcoord.y) + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y;
    }
    //if (MaskTexcoord.x > g_fTime * g_fMaskUVSpeed.x || MaskTexcoord.x < g_fTime * g_fMaskUVSpeed.x - 1)
    //    discard;
    //MaskTexcoord.x = fmod(MaskTexcoord.x, 1);
    if (0 > MaskTexcoord.y || 1 < MaskTexcoord.y)
        discard;
    if (0 > MaskTexcoord.x || 1 < MaskTexcoord.x)
        discard;
    Out.vColor.rgb *= max(g_DiffuseTexture.Sample(DefaultSampler, float2(MaskTexcoord.x, MaskTexcoord.y * 2)), 0.2);
    Out.vColor.a *= g_MaskTexture.Sample(DefaultSampler, float2(MaskTexcoord.x, MaskTexcoord.y * 2)).r * 10;
    Out.vColor.a *= saturate(g_fTime * 2);
    if (0 >= Out.vColor.a)
        discard;
    Out.vColor.rgb *= Out.vColor.a;
    Out.vColor.a = 1;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_LASER(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    MaskTexcoord.x += (1 - In.vTexcoord.y * g_fDiffuseUVSize.y);
    Out.vColor = g_vColor;
    //Out.vColor.rgb *= g_DiffuseTexture.Sample( DefaultSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y));
    
    float2 tex = MaskTexcoord + float2(g_fTime * g_fDiffuseUVSpeed.x, g_fTime * g_fDiffuseUVSpeed.y);
    
    float2 uv = MaskTexcoord;
    
    float3 noise = g_DiffuseTexture.Sample(YMirrorSampler, tex).rgb;
    
    float2 distort = (noise.rb * 2.0 - 1.0);
    
    float2 uvDistorted = MaskTexcoord + distort;
    
    float4 col = g_DissolveTexture.Sample(YMirrorSampler, uvDistorted);
    
    float mask = g_MaskTexture.Sample(YMirrorSampler, uvDistorted).r;
    
    Out.vColor.rgb *= col.rgb;
    
    Out.vColor.a *= mask;
    
    // * saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 3));
    //Out.vColor.a *= saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 3));
    //Out.vColor.a *= saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 5));
    //Out.vColor.a *= saturate(g_MaskTexture.Sample(YMirrorSampler, MaskTexcoord).r);
    //if (g_DissolveTexture.Sample(MirrorSampler, float2(DissolveTexcoord.x, DissolveTexcoord.y)).r + (1 - abs(MaskTexcoord.x * 1.1)) < abs(MaskTexcoord.x) + g_fTime * 2)
    //    discard;
    
    if (0 >= Out.vColor.a)
        discard;
    return Out;
}
// * max((abs(1 - saturate(g_fTime * 3))) * 4, 0.8)
/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_LASER_BLOOM(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    MaskTexcoord.x += (1 - In.vTexcoord.y * g_fDiffuseUVSize.y);
    Out.vColor = g_vColor;
    float2 tex = MaskTexcoord + float2(g_fTime * g_fDiffuseUVSpeed.x, g_fTime * g_fDiffuseUVSpeed.y);
    
    float2 uv = MaskTexcoord;
    
    float3 noise = g_DiffuseTexture.Sample(YMirrorSampler, tex).rgb;
    
    float2 distort = (noise.rb * 2.0 - 1.0);
    
    float2 uvDistorted = MaskTexcoord + distort;
    
    float4 col = g_DissolveTexture.Sample(YMirrorSampler, uvDistorted);
    
    float mask = g_MaskTexture.Sample(YMirrorSampler, uvDistorted).r;
    
    Out.vColor.rgb *= col.rgb;
    Out.vColor.rgb *= Out.vColor.a * mask; // * saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 5));
    //Out.vColor.a *= saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 5));
    //Out.vColor.rgb *= g_DiffuseTexture.Sample(DefaultSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y));
    //Out.vColor.a *= saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 5));
    //Out.vColor.a *= saturate(g_MaskTexture.Sample(YMirrorSampler, MaskTexcoord).r);
    if (0 >= Out.vColor.a)
        discard;
    Out.vColor.rgb = saturate(pow(Out.vColor.rgb * 10, 2));
    Out.vColor.rgb *= Out.vColor.a;
    Out.vColor.a = 1;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_LASER_LIGHTNING(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    MaskTexcoord.x += (1 - In.vTexcoord.y * g_fDiffuseUVSize.y);
    Out.vColor = g_vColor;
    
    float mask = g_MaskTexture.Sample(YMirrorSampler, MaskTexcoord).r;
    
    Out.vColor.a *= mask;
    
    // * saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 3));
    //Out.vColor.a *= saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 3));
    //Out.vColor.a *= saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 5));
    //Out.vColor.a *= saturate(g_MaskTexture.Sample(YMirrorSampler, MaskTexcoord).r);
    //if (g_DissolveTexture.Sample(MirrorSampler, float2(DissolveTexcoord.x, DissolveTexcoord.y)).r + (1 - abs(MaskTexcoord.x * 1.1)) < abs(MaskTexcoord.x) + g_fTime * 2)
    //    discard;
    
    if (0.7 >= Out.vColor.a)
        discard;
    return Out;
}
// * max((abs(1 - saturate(g_fTime * 3))) * 4, 0.8)
/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_LASER_LIGHTNING_BLOOM(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    MaskTexcoord.x += (1 - In.vTexcoord.y * g_fDiffuseUVSize.y);
    Out.vColor = g_vColor;
    
    float mask = g_MaskTexture.Sample(YMirrorSampler, MaskTexcoord).r;
    
    Out.vColor.a *= mask;
    
    Out.vColor.rgb *= Out.vColor.a * mask; // * saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 5));
    //Out.vColor.a *= saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 5));
    //Out.vColor.rgb *= g_DiffuseTexture.Sample(DefaultSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y));
    //Out.vColor.a *= saturate(pow((1 - abs(In.vTexcoord.y)) * 2, 5));
    //Out.vColor.a *= saturate(g_MaskTexture.Sample(YMirrorSampler, MaskTexcoord).r);
    if (0.7 >= Out.vColor.a)
        discard;
    Out.vColor.rgb = saturate(pow(Out.vColor.rgb * 10, 2));
    Out.vColor.rgb *= Out.vColor.a;
    Out.vColor.a = 1;
    return Out;
}

// * max((abs(1 - saturate(g_fTime * 3))) * 4, 0.8)
/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_SHOCK_MOTION_BLUR(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    matrix worldmat = g_CamMatrix;
    worldmat._11_12_13_14 = normalize(worldmat._11_12_13_14);
    worldmat._21_22_23_24 = normalize(worldmat._21_22_23_24);
    worldmat._31_32_33_34 = normalize(-worldmat._31_32_33_34);
    worldmat._41_42_43_44 = float4(0, 0, 0, 1);
    float4 pos = mul(g_WorldMatrix._41_42_43_44 - In.vWorldPos, worldmat) * 0.01f;
    pos.y *= -1;
    //pos.xy += pos.z;
    Out.vColor.xy = pos.xy;
    Out.vColor.zw = 0;
    return Out;
}


BlendState BS_BlendAlphaMax
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = max;
    SrcBlendAlpha = one;
    DestBlendAlpha = one;
    BlendOpAlpha = max;
};


technique11 DefaultTechnique
{ 
    // idx 0
    pass Normal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    // idx 1
    pass Distortion
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }
    // idx 2
    pass CircleDistortion
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CONE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CIRCLE_DISTORTION();
    }
    // idx 3
    pass Slash
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SLASH();
    }
    // idx 4
    pass Slash_Bloom
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SLASH_BLOOM();
    }
    // idx 5
    pass Hit
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CONE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HIT();
    }
    // idx 6
    pass Hit_Bloom
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CONE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HIT_BLOOM();
    }
    // idx 7
    pass Slash_RS
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SLASH_LOOP();
    }
    // idx 8
    pass Slash_RS_Bloom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SLASH_LOOP_BLOOM();
    }
    // idx 9
    pass Circle_Slash
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CIRCLE_SLASH();
    }
    // idx 10
    pass Circle_Slash_Bloom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CIRCLE_SLASH_BLOOM();
    }
    // idx 11
    pass Circle_Slash_Diffuse
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_BlendAlphaMax, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CIRCLE_SLASH_DIFFUSE();
    }
    // idx 12
    pass Circle_Slash_Diffuse_Bloom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CIRCLE_SLASH_DIFFUSE_BLOOM();
    }
    // idx 13
    pass Circle_Slash_Nonlight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_BlendAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CIRCLE_SLASH_NONLIGHT();
    }
    // idx 14
    pass Sting
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_BlendAlphaMax, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_STING();
    }
    // idx 15
    pass Sting_Bloom
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_STING_BLOOM();
    }
    // idx 16
    pass Sting_Shock
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_BlendAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_STING_SHOCK();
    }
    // idx 17
    pass Sting_Shock_Distortion
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_BlendAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_STING_SHOCK_DISTORTION();
    }
    // idx 18
    pass Circle_Slash_Loop
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_BlendAlphaMax, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CIRCLE_SLASH_LOOP();
    }
    // idx 19
    pass Circle_Slash_Loop_Bloom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CIRCLE_SLASH_LOOP_BLOOM();
    }
    // idx 20
    pass Laser
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_BlendAlphaMax, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LASER();
    }
    // idx 21
    pass Laser_Bloom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LASER_BLOOM();
    }
    // idx 22
    pass Laser_Lightning
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LASER_LIGHTNING();
    }
    // idx 23
    pass Laser_Lightning_Bloom
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LASER_LIGHTNING_BLOOM();
    }
    // idx 24
    pass Shock_Motion_Blur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SHOCK_MOTION_BLUR();
    }
}