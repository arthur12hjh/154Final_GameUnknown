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


float4 Calc_Normal(texture2D NormalTexture, float2 vTexcoord,
                   float3 vNormal, float3 vTangent, float3 vBinormal, float fScale = 1.f)
{
    // XY only normal map
    float2 xy = NormalTexture.SampleLevel(MirrorSampler, vTexcoord, 2.f).rg * 2.f - 1.f;

    xy *= fScale;
    // Z º¹¿ø
    float z = sqrt(saturate(1.f - dot(xy, xy)));

    float3 normalTS = float3(xy, z); // tangent-space normal

    float3x3 WorldMatrix = float3x3(vTangent,vBinormal * -1.f, vNormal);

    float3 vResultNormal = mul(normalTS, WorldMatrix);

    return float4(normalize(vResultNormal) * 0.5f + 0.5f, 0.f);
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

#endif