#ifndef __CLIENT_SHADER_VTXANIMMESH
#define __CLIENT_SHADER_VTXANIMMESH

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

struct VS_OUT_MOTIONBLUR
{
    float4 vPosition : SV_POSITION;
    float4 vDirection : TEXCOORD0;
};

struct PS_IN_MOTIONBLUR
{
    float4 vPosition : SV_POSITION;
    float4 vDirection : TEXCOORD0;
};

struct PS_OUT_MOTIONBLUR
{
    float4 vDirection : SV_TARGET0;
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
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
    float4 vORM : SV_Target3;
    float4 vEmissive : SV_TARGET4;
    float4 vSSSAO : SV_TARGET5;
    float4 vSpecDetail : SV_TARGET6;
};

struct PS_OUT_SHADOW
{
    float4 vShadowLightDepth : SV_TARGET0;
};


#endif