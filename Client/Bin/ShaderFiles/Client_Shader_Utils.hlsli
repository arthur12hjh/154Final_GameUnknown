#ifndef __CLIENT_SHADER_UTILS
#define __CLIENT_SHADER_UTILS

#include "Engine_Shader_Defines.hlsli"

float4 Calc_Normal(texture2D NormalTexture, float2 vTexcoord, float3 vNormal, float3 vTangent, float3 vBinormal)
{
    vector vNormalDesc = NormalTexture.Sample(MirrorSampler, vTexcoord);
    float3x3 WorldMatrix = float3x3(vTangent, vBinormal * -1.f, vNormal);
    float3 vResultNormal = mul(vNormalDesc.xyz * 2.f - 1.f, WorldMatrix);
 
    return float4(vResultNormal * 0.5f + 0.5f, 0.f);
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