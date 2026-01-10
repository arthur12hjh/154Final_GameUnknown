#include "Client_Shader_Utils.hlsli"
#include "Shader_Compute_CombinedMatrices.hlsl"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D           g_DiffuseTexture;
Texture2D           g_ORMTexture;
Texture2D           g_NormalTexture;

vector               g_vCamPosition;
float                g_fRimLightPower;
float                g_fRimLightStrength;
float4               g_vRimLightColor;

int                  g_iNumBone;
float4               g_vStartColor;
float4               g_vEndColor;
matrix               g_OffsetMatrices[512];

StructuredBuffer<BoneTransformMatrix>           g_BoneMatrixBuffer : register(t18);

struct VS_MOTION_TRAIL_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
    
    row_major float4x4  TransformMatrix : WORLD;
    uint2               iIndexNumber : TEXCOORD1;
    float2              vLifeTime : TEXCOORD2;
};

struct VS_MOTION_TRAIL_OUT
{
    float4 vPosition    : SV_POSITION;
    float3 vNormal      : NORMAL;
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
    float2 vTexcoord    : TEXCOORD0;
    float4 vWorldPos    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
    uint2  iIndex       : TEXCOORD3;
    float2 vLifeTime    : TEXCOORD4;
};

VS_MOTION_TRAIL_OUT VS_MOTION_TRAIL_MAIN(VS_MOTION_TRAIL_IN In)
{
    VS_MOTION_TRAIL_OUT Out;
    float fWeightSum = (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z + In.vBlendWeight.w);
    
    float fWeightX = 1;
    float fWeightY = 0;
    float fWeightZ = 0;
    float fWeightW = 0;
    
    if (fWeightSum > 1e-6f)
    {
        fWeightX = In.vBlendWeight.x / fWeightSum;
        fWeightY = In.vBlendWeight.y / fWeightSum;
        fWeightZ = In.vBlendWeight.z / fWeightSum;
        fWeightW = In.vBlendWeight.w / fWeightSum;
    }
     
    int iIndex = In.iIndexNumber.y * g_iNumBone;
    float4x4 MatrixX = mul(g_OffsetMatrices[In.vBlendIndex.x], g_BoneMatrixBuffer[iIndex + In.vBlendIndex.x].BoneCombinedTransformMatrix);
    float4x4 MatrixY = mul(g_OffsetMatrices[In.vBlendIndex.y], g_BoneMatrixBuffer[iIndex + In.vBlendIndex.y].BoneCombinedTransformMatrix);
    float4x4 MatrixZ = mul(g_OffsetMatrices[In.vBlendIndex.z], g_BoneMatrixBuffer[iIndex + In.vBlendIndex.z].BoneCombinedTransformMatrix);
    float4x4 MatrixW = mul(g_OffsetMatrices[In.vBlendIndex.w], g_BoneMatrixBuffer[iIndex + In.vBlendIndex.w].BoneCombinedTransformMatrix);
    
    matrix BoneMatrix = MatrixX * fWeightX +
                        MatrixY * fWeightY +
                        MatrixZ * fWeightZ +
                        MatrixW * fWeightW;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    float3 vSkinnedNormal = mul(float4(In.vNormal, 0.f), BoneMatrix).xyz;
    float3 vSkinnedTangent = mul(float4(In.vTangent, 0.f), BoneMatrix).xyz;
    float3 vSkinnedBinorm = mul(float4(In.vBinormal, 0.f), BoneMatrix).xyz;
    
    matrix matWV, matWVP;
    
    matWV = mul(In.TransformMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = normalize(mul(vector(vSkinnedNormal, 0.f), In.TransformMatrix)).xyz;
    Out.vTangent = normalize(mul(vector(vSkinnedTangent, 0.f), In.TransformMatrix)).xyz;
    Out.vBinormal = normalize(mul(vector(vSkinnedBinorm, 0.f), In.TransformMatrix)).xyz;
    Out.vWorldPos = mul(vPosition, In.TransformMatrix);
    
    Out.vProjPos = Out.vPosition;
    Out.iIndex = In.iIndexNumber;
    Out.vLifeTime = In.vLifeTime;
    return Out;
}

struct PS_MOTION_TRAIL_IN
{
    float4 vPosition    : SV_POSITION;
    float3 vNormal      : NORMAL;
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
    float2 vTexcoord    : TEXCOORD0;
    float4 vWorldPos    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
    
    uint2  iIndex       : TEXCOORD3;
    float2 vLifeTime    : TEXCOORD4;
};

struct PS_MOTION_TRAIL_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vORM : SV_Target3;
    float4 vEmissive : SV_TARGET4;
    float4 vBloom : SV_TARGET5;
};


/* ÇÈ¼¿ ½¦ÀÌ´õ : ÇÈ¼¿ÀÇ ÃÖÁ¾ÀûÀÎ »öÀ» °áÁ¤ÇÏ³®. */
PS_MOTION_TRAIL_OUT PS_MOTION_TRAIL_MAIN(PS_MOTION_TRAIL_IN In)
{
    PS_MOTION_TRAIL_OUT Out;
    
    if (In.vLifeTime.x >= In.vLifeTime.y)
        discard;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vMtrlDiffuse = float4(1.f, 0.f, 0.f, 1 - In.vLifeTime);
    Out.vDiffuse = vMtrlDiffuse +
        Calc_RimLight(g_fRimLightStrength, g_fRimLightPower, g_vCamPosition, g_vRimLightColor, In.vNormal, In.vWorldPos);
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    return Out;
}

technique11 DefaultTechnique
{
    pass MOTION_TRAIL
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MOTION_TRAIL_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MOTION_TRAIL_MAIN();
    }
}