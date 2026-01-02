#include "Client_Shader_Utils.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_ORMTexture;
Texture2D g_MaskTexture;

float g_fFar;
float g_fTime;
float g_fWaveSpeed;
float g_fWaveFrequency;
float g_fWaveAmplitude;
float3 g_fMinMaxScale;
int g_iSizeCount;
bool g_bisSpectrum;


vector g_vColor = vector(1.f, 1.f, 1.f, 1.f);
float2 g_fMaskUV = float2(0, 0);
float2 g_fMaskUVSpeed = float2(0, 0);
float2 g_fMaskUVSize = float2(1, 1);
float2 g_fDiffuseUV = float2(0, 0);
float2 g_fDiffuseUVSpeed = float2(0, 0);
float2 g_fDiffuseUVSize = float2(1, 1);
float2 g_fDissolveUV = float2(0, 0);
float2 g_fDissolveUVSpeed = float2(0, 0);
float2 g_fDissolveUVSize = float2(1, 1);

StructuredBuffer<float3> g_fSizeDiagram : register(t0);

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
    
    float2 vLifeTime : TEXCOORD1;
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
    float4 vOriginalWorldPos : TEXCOORD3;
    float2 vLifeTime : TEXCOORD4;
    int vSeed : SEED;
};


struct PS_NONLIGHT_OUT
{
    float4 vDiffuse : SV_TARGET0;
};

VS_OUT VS_MAIN(VS_IN In, uint id : SV_InstanceID)
{
    VS_OUT Out;
    
    
    
    float time = In.vLifeTime.x / In.vLifeTime.y;
    float3 fInTime = float3(0, 0, 0);
    float3 fOutTime = float3(-1, 0, 0);
    float fSize = 0.f;
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
        fSize = length(In.TransformMatrix._11_12_13) * fInTime.y;
    else
    {
        float t = (time - fInTime.x) / (fOutTime.x - fInTime.x);
        fSize = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * fInTime.y
     + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(fInTime.z)) * (fOutTime.x - fInTime.x) * 100
     + (-2 * pow(t, 3) + 3 * pow(t, 2)) * fOutTime.y
     + (pow(t, 3) - pow(t, 2)) * tan(radians(fOutTime.z)) * (fOutTime.x - fInTime.x) * 100;
        fSize *= length(In.TransformMatrix._11_12_13);
    }
    
    vector vPosition = mul(vector(In.vPosition * 0.001 * fSize, 1.f), In.TransformMatrix);
    vector vNormal = mul(vector(In.vNormal * 0.001 * fSize, 0.f), In.TransformMatrix);
    vector vTangent = mul(vector(In.vTangent * 0.001 * fSize, 0.f), In.TransformMatrix);
    vector vBINormal = mul(vector(In.vBInormal * 0.001 * fSize, 0.f), In.TransformMatrix);
    
    Out.vOriginalWorldPos = float4(0.f, 0.f, 0.f, 0.f);
    
    /* In.vPosition * 월드 * 뷰 * 투영 */    
    //float4x4 == matrix
    matrix matWV, matWVP;
    
   // vector vPosition = mul(vector(0.f, 0.f, 0.f, 1.f), In.TransformMatrix);
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    
    if (g_bisSpectrum)
    {
        float4 matPos = mul(vPosition, g_ViewMatrix);
        Out.vPosition = mul(matPos, g_ProjMatrix);
    }
    else{
        Out.vPosition = mul(vPosition, matWVP);
    }
    //Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(vTangent, g_WorldMatrix)).xyz;
    Out.vBINormal = normalize(mul(vBINormal, g_WorldMatrix)).xyz;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vLifeTime = In.vLifeTime;
    Out.vSeed = id;
    return Out;
}

float Random(float2 v)
{
    return frac(sin(dot(v, float2(12.9898, 78.233))) * 43758.5453123);
}

//VS_OUT VS_MAIN_REED(VS_IN In)
//{
//    vector vPosition = vector(In.vPosition, 1.f);
//    vector vTranslation = In.TransformMatrix[3];
//    
//    VS_OUT Out;
//    
//    Out.vOriginalWorldPos = mul(vector(In.vPosition, 1.f), In.TransformMatrix);
//    
//    float fScale = g_fMinMaxScale.x + Random(vTranslation.xz) * (g_fMinMaxScale.y - g_fMinMaxScale.x);
//    
//    vPosition.xyz *= float3(1.f, fScale, 1.f) * 0.001;;
//    
//    float fTimeOffSet = Random(vTranslation.xz);
//    float fNewTime = g_fTime + fTimeOffSet * 100.f;
//    
//    // 1. 흔들림 
//    float waveOffset = sin(fNewTime * g_fWaveSpeed + (vPosition.x + vPosition.z) * g_fWaveFrequency);
//    
//    // 2. 높이에 비례 흔들림 강도 조절 
//    float fHeightFactor = clamp(vPosition.y / 1.5f, 0.0f, 1.0f);
//    
//    // 3. 변위 적용 (X축)
//    float fDisplacement = waveOffset * g_fWaveAmplitude * fHeightFactor;
//    vPosition.x += fDisplacement;
//    
//    float fRand = Random(vTranslation.xz);
//    float fMinAngle = radians(135.f);
//    float fMaxAngle = radians(225.f);
//    float fRange = fMaxAngle - fMinAngle;
//    
//    float fRotationAngle = fMinAngle + fRand * fRange;
//    
//    float sinA = sin(fRotationAngle);
//    float cosA = cos(fRotationAngle);
//    
//    matrix matFixedRotation = (matrix) 0;
//    
//    matFixedRotation[0][0] = cosA;
//    matFixedRotation[0][2] = -sinA;
//    matFixedRotation[1][1] = 1.f;
//    matFixedRotation[2][0] = sinA;
//    matFixedRotation[2][2] = cosA;
//    
//    matFixedRotation[3] = vTranslation;
//    
//    /* In.vPosition * 월드 * 뷰 * 투영 */    
//    //float4x4 == matrix
//    matrix matWV, matWVP;
//    
//    vPosition = mul(vPosition, matFixedRotation);
//    matWV = mul(g_WorldMatrix, g_ViewMatrix);
//    matWVP = mul(matWV, g_ProjMatrix);
//    
//    Out.vPosition = mul(vPosition, matWVP);
//    Out.vTexcoord = In.vTexcoord;
//    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
//    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
//    Out.vBINormal = normalize(mul(vector(In.vBInormal, 0.f), g_WorldMatrix)).xyz;
//    //Out.vWorldPos = mul(vector(In.vPosition, 1.f), matFixedRotation);
//    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
//    Out.vProjPos = Out.vPosition;
//    Out.vLifeTime = In.vLifeTime;
//
//    return Out;
//}

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
    float4 vOriginalWorldPos : TEXCOORD3;
    float2 vLifeTime : TEXCOORD4;
    int vSeed : SEED;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vORM : SV_Target3;
    float4 vEmissive : SV_TARGET4;
};


float hole[16] =
{
    0, 8, 2, 10,
    12, 4, 14, 6,
     3, 11, 1, 9,
    15, 7, 13, 5
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    if (In.vLifeTime.x <= 0 || In.vLifeTime.x > In.vLifeTime.y)
        discard;
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
    
    int index = (int(In.vPosition.x) & 3) + (int(In.vPosition.y) & 3) * 4;
    
    float threshold = hole[index] / 32.0;
    if (0 >= vMtrlDiffuse.a * saturate((In.vLifeTime.y - In.vLifeTime.x)) - threshold || 0 >= In.vLifeTime.y - In.vLifeTime.x)
        discard;
    
    vector vNormalTexture = g_NormalTexture.Sample(MirrorSampler, In.vTexcoord);
    float3x3 TangentSpaceMat = float3x3(In.vTangent, In.vBINormal * -1, In.vNormal);
    float3 vNormal = mul(vNormalTexture.xyz * 2.f - 1.f, TangentSpaceMat);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBINormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 1.0f);
    Out.vORM = g_ORMTexture.Sample(MirrorSampler, In.vTexcoord);
    Out.vEmissive = vMtrlDiffuse * g_vColor;
    //Out.vDiffuse = vMtrlDiffuse;
    //Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 0.f);
    //Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.0f, 0.0f);
    //Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    
    return Out;
}

PS_OUT PS_SPRITE(PS_IN In, bool bisFront : SV_IsFrontFace)
{
    PS_OUT Out;
    if (In.vLifeTime.x <= 0 || In.vLifeTime.x > In.vLifeTime.y)
        discard;
    
    int i = In.vSeed % (g_fDiffuseUVSize.x * g_fDiffuseUVSize.y);
    int iU = i % g_fDiffuseUVSize.x;
    int iV = i / g_fDiffuseUVSize.x;
    float2 fTexcoord = float2(In.vTexcoord.x / g_fDiffuseUVSize.x + 1.0 / g_fDiffuseUVSize.x * iU, In.vTexcoord.y / g_fDiffuseUVSize.y + 1.0 / g_fDiffuseUVSize.y * iV);
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, fTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
    
    int index = (int(In.vPosition.x) & 3) + (int(In.vPosition.y) & 3) * 4;
    
    float threshold = hole[index] / 32.0;
    if (0 >= vMtrlDiffuse.a * saturate((In.vLifeTime.y - In.vLifeTime.x)) - threshold || 0 >= In.vLifeTime.y - In.vLifeTime.x)
        discard;
    Out.vDiffuse = vMtrlDiffuse;
    float3 normal = normalize(In.vNormal);

    if (!bisFront)
        normal = -normal;

    Out.vNormal = float4(normal * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 1.0f);
    Out.vORM = g_ORMTexture.Sample(MirrorSampler, In.vTexcoord);
    Out.vEmissive = vMtrlDiffuse * g_vColor;
    
    return Out;
}

PS_OUT PS_WHITE(PS_IN In)
{
    PS_OUT Out;
    if (In.vLifeTime.x <= 0 || In.vLifeTime.x > In.vLifeTime.y)
        discard;
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
    
    int index = (int(In.vPosition.x) & 3) + (int(In.vPosition.y) & 3) * 4;
    
    float threshold = hole[index] / 32.0;
    if (0 >= vMtrlDiffuse.a * saturate((In.vLifeTime.y - In.vLifeTime.x)) - threshold || 0 >= In.vLifeTime.y - In.vLifeTime.x)
        discard;
    
    float3 normal = normalize(In.vNormal);
    
    Out.vDiffuse = float4(1, 1, 1, 1);

    Out.vNormal = float4(normal * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 1.0f);
    Out.vORM = float4(0, 0, 0, 0);
    Out.vEmissive = vMtrlDiffuse * g_vColor;
    
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
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.0f, 0.0f);
    return Out;
}

technique11 Tech
{
    // idx 0
    pass Origin_MeshParticle
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    // idx 1
    pass MeshParticle
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SPRITE();
    }
    // idx 2
    pass MeshWhite
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WHITE();
    }
}