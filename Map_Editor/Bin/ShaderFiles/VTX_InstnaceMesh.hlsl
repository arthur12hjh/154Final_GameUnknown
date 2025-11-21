#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D   g_DiffuseTexture;
Texture2D   g_NormalTexture;

float       g_CamFar;
float       g_fTime;
float       g_fWaveSpeed;
float       g_fWaveFrequency;
float       g_fWaveAmplitude;
float3      g_fMinMaxScale;

/* 정점 쉐이더 : */
/* 정점에 대한 셰이딩 == 정점에 필요한 연산을 수행한다 == 정점의 상태변환(월드, 뷰, 투영) + 추가변환 */
/* 정점의 구성 정보를 수정, 변경한다 */ 
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBInormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    
    row_major float4x4 TransformMatrix : WORLD;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBINormal : BINORMAL;
    
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    /* In.vPosition * 월드 * 뷰 * 투영 */    
    //float4x4 == matrix
    matrix  matWV, matWVP;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBINormal = normalize(mul(vector(In.vBInormal, 0.f), g_WorldMatrix)).xyz;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;

    return Out;
}

float Random(float2 v)
{
    return frac(sin(dot(v, float2(12.9898, 78.233))) * 43758.5453123);
}

VS_OUT VS_MAIN_REED(VS_IN In)
{
    vector vPosition = vector(In.vPosition, 1.f);
    vector vTranslation = In.TransformMatrix[3];
    
    float fScale = g_fMinMaxScale.x + Random(vTranslation.xz) * (g_fMinMaxScale.y - g_fMinMaxScale.x);
    vPosition.xyz *= float3(1.f, fScale, 1.f);
    
    float fTimeOffSet = Random(vTranslation.xz);
    float fNewTime = g_fTime + fTimeOffSet * 100.f;
    
    // 1. 흔들림 
    float waveOffset = sin(fNewTime * g_fWaveSpeed + (vPosition.x + vPosition.z) * g_fWaveFrequency);
    
    // 2. 높이에 비례 흔들림 강도 조절 
    float fHeightFactor = clamp(vPosition.y / 1.5f, 0.0f, 1.0f);
    
    // 3. 변위 적용 (X축)
    float fDisplacement = waveOffset * g_fWaveAmplitude * fHeightFactor;
    vPosition.x += fDisplacement;
    
    VS_OUT Out;
    
    
    float fRand = Random(vTranslation.xz);
    float fMinAngle = radians(-45.f);
    float fMaxAngle = radians(45.f);
    float fRange = fMaxAngle - fMinAngle;
    
    float fRotationAngle = fMinAngle + fRand * fRange;
    
    float sinA = sin(fRotationAngle);
    float cosA = cos(fRotationAngle);
    
    matrix matFixedRotation = (matrix)0;
    
    matFixedRotation[0][0] = cosA;
    matFixedRotation[0][2] = -sinA;
    matFixedRotation[1][1] = 1.f;
    matFixedRotation[2][0] = sinA;
    matFixedRotation[2][2] = cosA;
    
    matFixedRotation[3] = vTranslation;
    
    /* In.vPosition * 월드 * 뷰 * 투영 */    
    //float4x4 == matrix
    matrix matWV, matWVP;
    
    vPosition = mul(vPosition, matFixedRotation);
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBINormal = normalize(mul(vector(In.vBInormal, 0.f), g_WorldMatrix)).xyz;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), matFixedRotation);
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
    float3 vBINormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
    
    vector vNoramlTexture = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3x3 TangentSpaceMat = float3x3(In.vTangent, In.vBINormal * -1, In.vNormal);
    float3 vNormal = mul(vNoramlTexture.xyz * 2.f - 1.f, TangentSpaceMat);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_CamFar, 0.0f, 0.0f);
    return Out;
}

struct PS_OUT_NONE_NORMAL
{
    float4 vDiffuse : SV_TARGET0;
    float4 vDepth : SV_TARGET2;
};

PS_OUT_NONE_NORMAL PS_None_Normal(PS_IN In)
{
    PS_OUT_NONE_NORMAL Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_CamFar, 0.0f, 0.0f);
    return Out;
}

technique11 Tech
{
    pass Pass0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    
    pass Pass1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_None_Normal();
    }
    
    pass Reed
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN_REED();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }


}