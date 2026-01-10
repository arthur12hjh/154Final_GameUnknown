#ifndef __CLIENT_SHADER_UTILS
#define __CLIENT_SHADER_UTILS

#include "Engine_Shader_Defines.hlsli"

#define CASCADE_LEVEL 5

struct GS_OUT_SHADOW
{
    float4 vPosition : SV_Position;
    float4 vProjPos : TEXCOORD0; 
    uint iSlice : SV_RenderTargetArrayIndex;
};

struct PS_IN_SHADOW
{
    float4 vPosition : SV_Position;
    float4 vProjPos : TEXCOORD0;
    uint iSlice : SV_RenderTargetArrayIndex;
};

float4 Calc_TerrainNormal(texture2D NormalTexture, float2 vTexcoord, float3 vNormal)
{
    //노말 벡터밖에 없으니까 탄젠트랑 바이노말을 만들어주자..
    float3 vTangent = cross(float3(0.f, 1.f, 0.f), vNormal);
    float3 vBinormal = cross(vNormal, vTangent);
   
    
    float2 vXY = NormalTexture.SampleLevel(MirrorSampler, vTexcoord, 2.f).rg * 2.f - 1.f;
    float fZ = sqrt(saturate(1.f - dot(vXY, vXY)));
    
    float3 vNormalTS = float3(vXY, fZ); // tangent-space normal

    float3x3 WorldMatrix = float3x3(vTangent, vBinormal * -1.f, vNormal);
    float3 vResultNormal = mul(vNormalTS, WorldMatrix);
    
    return float4(normalize(vResultNormal) * 0.5f + 0.5f, 0.f);
}

float4 Calc_RGBNormal(texture2D NormalTexture, float2 vTexcoord,
                   float3 vNormal, float3 vTangent, float3 vBinormal, float fScale = 1.f)
{
    vector vNormalDesc = NormalTexture.Sample(MirrorSampler, vTexcoord);
    float3x3 WorldMatrix = float3x3(vTangent, vBinormal, vNormal);
    
    float3 vResultNormal = mul(vNormalDesc.xyz * 2.f - 1.f, WorldMatrix);
   
    return float4(vResultNormal * 0.5f + 0.5f, 0.f);
}

float4 Calc_Normal(texture2D NormalTexture, float2 vTexcoord,
                                 float3 vNormal, float3 vTangent, float3 vBinormal,
                                 float fScale = 1.f)
{
    float2 xy = NormalTexture.Sample(NormalSampler, vTexcoord).rg * 2.f - 1.f;
    xy *= fScale;

    float z = sqrt(saturate(1.f - dot(xy, xy)));
    float3 nTS = float3(xy, z);

    float3x3 TBN = float3x3(normalize(vTangent), normalize(vBinormal), normalize(vNormal));

    float3 nWS = mul(nTS, TBN);
    nWS = normalize(nWS);

    return float4(nWS * 0.5f + 0.5f, 0.f);
}

vector Calc_ORSS(texture2D ORSSTexture, float2 vTexcoord)
{
    vector vORSS = ORSSTexture.Sample(DefaultSampler, vTexcoord);
    return float4(vORSS.r, vORSS.g, vORSS.b, vORSS.a);
}

vector Calc_ORM(texture2D ORMTexture, float2 vTexcoord)
{
    vector vORM = ORMTexture.Sample(DefaultSampler, vTexcoord);
    return float4(vORM.r, vORM.g, vORM.b, 0.f);
}

vector Calc_Emissive(texture2D EmissiveMask, vector vDiffuse, float2 vTexCoord)
{
    vector vMask = EmissiveMask.Sample(DefaultSampler, vTexCoord);
    
    if(vMask.r != 0)
        return vDiffuse * vMask.r;
    else
        return float4(0.f, 0.f, 0.f, 0.f);
}

vector Calc_RimLight(float fStrength, float fPower, vector vCamPos, vector vRimColor, vector vNormal, vector vWorldPos)
{
    vector vPosToCam = normalize(vCamPos - vWorldPos);
    
    vector vRimLight = (1 - dot(normalize(vNormal), vPosToCam));
    
    vRimLight = pow(vRimLight, fPower) * fStrength * vRimColor;

    return vRimLight;
}

float Calc_RimLightPower(float fStrength, float fPower, vector vCamPos, vector vRimColor, vector vNormal, vector vWorldPos)
{
    vector vPosToCam = normalize(vCamPos - vWorldPos);

    return pow((1 - dot(normalize(vNormal), vPosToCam)), fPower) * fStrength;
}

#endif