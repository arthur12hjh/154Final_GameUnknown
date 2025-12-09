
#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
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

texture2D g_MaskTexture, g_DiffuseTexture, g_DissolveTexture;
int g_iSizeCount;
StructuredBuffer<float3> g_fSizeDiagram : register(t0);
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

struct PS_NONLIGHT_OUT
{
    float4 vColor : SV_TARGET0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vWeight : SV_TARGET1;
};

struct PS_NORMAL_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};



/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    
    //Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r;
    //Out.vDiffuse.a *= ((fireFront.r + fireBack.r) / 2) * ((In.vLifeTime.y - In.vLifeTime.x) / In.vLifeTime.y);
    
    float linearDepth = 0.1 * 500 / (500.f - (In.vProjPos.z / In.vProjPos.w) * (500 - 0.1));
    float weight = saturate(pow(1 - linearDepth / 500, 3));
    float time = g_fTime;
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + min(time * g_fMaskUVSpeed.x, 1)) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + time * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + time * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + time * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + time * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + time * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    //if (In.vTexcoord.y < 0.5)
    //{
        Out.vColor = g_vColor;
        vector color = g_DiffuseTexture.Sample(MirrorSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y)) * g_vColor;
        color.a = (1 - abs(MaskTexcoord.x * 1.2));
        Out.vColor.rgb = Out.vColor.rgb * (1 - color.a) + color.rgb * color.a;
    Out.vColor.a *= g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x * max((abs(1 - saturate(time * 3))) * 4, 0.8), MaskTexcoord.y)).r;
        if (g_DissolveTexture.Sample(MirrorSampler, float2(DissolveTexcoord.x, DissolveTexcoord.y)).r + (1 - abs(MaskTexcoord.x * 1.1)) < abs(MaskTexcoord.x) + time * 2)
            discard;
    
    //}
    //else
    //{
    //    Out.vColor = g_vColor;
    //    //Out.vDiffuse *= (1 - MaskTexcoord.x) * (1 - (MaskTexcoord.y - 0.5) * 2);
    //    vector color = g_DiffuseTexture.Sample(DefaultSampler, float2(DiffuseTexcoord.x, 1 - (DiffuseTexcoord.y - 0.5) * 2)) * g_vColor;
    //    color.a = (1 - abs(MaskTexcoord.x * 1.2));
    //    Out.vColor.rgb = Out.vColor.rgb * (1 - color.a) + color.rgb * color.a;
    //    Out.vColor.a = g_vColor.a * g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x * max((abs(1 - time * 3.5)) * 4, 1), 1 - (MaskTexcoord.y - 0.5) * 2)).r;
    //    
    //    if (g_DissolveTexture.Sample(MirrorSampler, float2(DissolveTexcoord.x, 1 - (DissolveTexcoord.y - 0.5) * 2)).r + (1 - abs(MaskTexcoord.x * 1.1)) < abs(MaskTexcoord.x) + time * 2.5)
    //        discard;
    //}
    
    //float maxColor = max(max(Out.vColor.r, Out.vColor.g), Out.vColor.b);
    //Out.vColor.r = 1 - (maxColor - Out.vColor.r);
    //Out.vColor.g = 1 - (maxColor - Out.vColor.g);
    //Out.vColor.b = 1 - (maxColor - Out.vColor.b);
    //Out.vColor.a -= (1 - maxColor);
    //if (Out.vColor.a <= 0.1f || 0 > fmod(g_fTime, 1))
    //    discard;
    
    if (0 >= Out.vColor.a)
        discard;
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a * weight;
    Out.vWeight.r = Out.vColor.a * weight;
    Out.vWeight.g = Out.vColor.a;
    Out.vWeight.b = 1 - weight;
    Out.vColor.a = 1;
    Out.vWeight.a = 1;
    
    //Out.vNormal = float4(In.vNormal * 0.5f + 0.5f, 1.f);
    //Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 1.0f);
    return Out;
}
/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_DISTORTION(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    if (0 > MaskTexcoord.x || 1 < MaskTexcoord.x || 0 > MaskTexcoord.y || 1 < MaskTexcoord.y)
        discard;
    Out.vColor = g_vColor;
    Out.vColor *= (1 - abs(MaskTexcoord.x)) * abs(MaskTexcoord.y) * 2;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_Hit(PS_IN In)
{
    PS_OUT Out;
    
    float linearDepth = 0.1 * 500 / (500.f - (In.vProjPos.z / In.vProjPos.w) * (500 - 0.1));
    float weight = saturate(pow(1 - linearDepth / 500, 3));
    float time = g_fTime;
    
    
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
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a * weight;
    Out.vWeight.r = Out.vColor.a * weight;
    Out.vWeight.g = Out.vColor.a;
    Out.vWeight.b = 1 - weight;
    Out.vColor.a = 1;
    Out.vWeight.a = 1;
    
    return Out;
}
/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_CIRCLE_DISTORTION(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    if (0 > MaskTexcoord.y || 1 < MaskTexcoord.y)
        discard;
    Out.vColor = g_vColor;
    Out.vColor *= abs(MaskTexcoord.y) * 2;
    return Out;
}

float hole[16] =
{
    0, 8, 2, 10,
    12, 4, 14, 6,
     3, 11, 1, 9,
    15, 7, 13, 5
};

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NORMAL_OUT PS_NORMAL_COLOR(PS_IN In)
{
    PS_NORMAL_OUT Out;
    
    
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    if (0 > MaskTexcoord.y || 1 < MaskTexcoord.y)
        discard;
    Out.vDiffuse = g_vColor;
    //Out.vDiffuse *= abs(MaskTexcoord.y) * 2;
    Out.vNormal = float4(In.vNormal * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 1.0f);
    return Out;
}

PS_NONLIGHT_OUT PS_COLOR(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    Out.vColor = g_vColor;
    return Out;
}

technique11 DefaultTechnique
{ 
    pass UI
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Distortion
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }

    pass Hit
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CONE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Hit();
    }

    pass CircleDistortion
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CONE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CIRCLE_DISTORTION();
    }

    pass Mesh
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NORMAL_COLOR();
    }


 
    

 
}