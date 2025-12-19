#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_CamMatrix;

texture2D g_MaskTexture, g_DiffuseTexture, g_NormalTexture;
texture2D g_DepthTexture;

vector g_vColor = vector(1.f, 1.f, 1.f, 1.f);
float2 g_fSize;
float2 g_fMaskUV = float2(0, 0);
float2 g_fMaskUVSpeed = float2(0, 0);
float2 g_fMaskUVSize = float2(1, 1);
float2 g_fDiffuseUV = float2(0, 0);
float2 g_fDiffuseUVSpeed = float2(0, 0);
float2 g_fDiffuseUVSize = float2(1, 1);
float2 g_fDissolveUV = float2(0, 0);
float2 g_fDissolveUVSpeed = float2(0, 0);
float2 g_fDissolveUVSize = float2(1, 1);
int2 g_iUV;
float g_fAngle;
float g_fFar;
int g_iSizeCount;
int g_iBillboard;
bool g_bisSpectrum;
bool g_bisAnimation;

StructuredBuffer<float3> g_fSizeDiagram : register(t0);

struct VS_IN
{
    float3 vPosition : POSITION;
    
    row_major float4x4 TransformMatrix : WORLD;
    
    float2 vLifeTime : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    row_major float4x4 TransformMatrix : WORLD;
    float fSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vOnePos : TEXCOORD2;
    int vSeed : SEED;
};

VS_OUT VS_MAIN(VS_IN In, uint id : SV_InstanceID)
{
    VS_OUT Out;
   
    vector vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);
    
    if (g_bisSpectrum)
        Out.vPosition = vPosition;
    else
        Out.vPosition = mul(vPosition, g_WorldMatrix);
    
    
    float time = In.vLifeTime.x / In.vLifeTime.y;
    float3 fInTime = float3(0, 0, 0);
    float3 fOutTime = float3(-1, 0, 0);
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
        Out.fSize = length(In.TransformMatrix._11_12_13) * fInTime.y;
    else
    {
        float t = (time - fInTime.x) / (fOutTime.x - fInTime.x);
        float fSize = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * fInTime.y
     + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(fInTime.z)) * (fOutTime.x - fInTime.x) * 100
     + (-2 * pow(t, 3) + 3 * pow(t, 2)) * fOutTime.y
     + (pow(t, 3) - pow(t, 2)) * tan(radians(fOutTime.z)) * (fOutTime.x - fInTime.x) * 100;
        Out.fSize = length(In.TransformMatrix._11_12_13) * fSize;
    }
    Out.vLifeTime = In.vLifeTime;
    Out.vProjPos = Out.vPosition;
    Out.vOnePos = vector(In.vPosition, 1);
    Out.vSeed = id;
    Out.TransformMatrix = In.TransformMatrix;
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    row_major float4x4 TransformMatrix : WORLD;
    float fSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vOnePos : TEXCOORD2;
    int vSeed : SEED;
};

struct GS_NORMAL_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vNormal : TEXCOORD3;
    float4 vTangent : TEXCOORD4;
    float4 vBitangent : TEXCOORD5;
    int vSeed : SEED;
};

struct GS_NONLIGHT_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    int vSeed : SEED;
};

struct GS_WEIGHT_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    int vSeed : SEED;
};

[maxvertexcount(6)]
void GS_NORMAL_BILLBOARD(point GS_IN In[1], inout TriangleStream<GS_NORMAL_OUT> OutStream)
{
    GS_NORMAL_OUT Out[4];
    float3 vRightRot;
    float3 vLookRot;
    float3 vR = 0;
    float3 vL = 0;
    float angle = radians(g_fAngle);
    float s = sin(angle);
    float c = cos(angle);
    switch (g_iBillboard % 4)
    {
        case 0:
            {
                float3 vLook = normalize(In[0].TransformMatrix._31_32_33);
                float3 vRight = normalize(In[0].TransformMatrix._11_12_13);
    
                vRightRot = vRight * c + vLook * s;
                vLookRot = vLook * c - vRight * s;
                if (!g_bisSpectrum)
                {
                    vRightRot = normalize(mul(float4(vRightRot, 0), g_WorldMatrix)).xyz;
                    vLookRot = normalize(mul(float4(vLookRot, 0), g_WorldMatrix)).xyz;
                }
    
                vR = vRightRot * (g_fSize.x * In[0].fSize * 0.5f);
                vL = vLookRot * (g_fSize.y * In[0].fSize * 0.5f);
            }
            break;
        case 1:
            {
                float3 vRight = -normalize(g_CamMatrix._11_12_13);
                float3 vUp = normalize(g_CamMatrix._21_22_23);
    
                vRightRot = vRight * c + vUp * s;
                vLookRot = vUp * c - vRight * s;
    
                vR = vRightRot * (g_fSize.x * In[0].fSize * 0.5f);
                vL = vLookRot * (g_fSize.y * In[0].fSize * 0.5f);
            }
            break;
        case 2:
            {
                float3 vLook = normalize(In[0].TransformMatrix._31_32_33);
                float3 vRight = normalize(float4(cross(normalize(g_CamMatrix._31_32_33), vLook), 0));
    
                vRightRot = vRight * c + vLook * s;
                vLookRot = vLook * c - vRight * s;
                if (!g_bisSpectrum)
                {
                    vRightRot = normalize(mul(float4(vRightRot, 0), g_WorldMatrix)).xyz;
                    vLookRot = normalize(mul(float4(vLookRot, 0), g_WorldMatrix)).xyz;
                }
    
                vR = vRightRot * (g_fSize.x * In[0].fSize * 0.5f);
                vL = vLookRot * (g_fSize.y * In[0].fSize * 0.5f);
            }
            break;
        case 3:
            {
                float3 vRight = -normalize(g_CamMatrix._11_12_13);
                float3 vUp = normalize(g_CamMatrix._21_22_23);
                float3 localUp = normalize(float3(
    In[0].TransformMatrix._31,
    In[0].TransformMatrix._32,
    In[0].TransformMatrix._33));
        
                float projRight = dot(localUp, vRight);
                float projUp = dot(localUp, vUp);
        
        
        
                float3 up = normalize(mul(float3(In[0].TransformMatrix._31, In[0].TransformMatrix._32, In[0].TransformMatrix._33), g_CamMatrix._31_32_33));
                float angle = atan2(projUp, projRight) + radians(g_fAngle);
    
                float s = sin(angle);
                float c = cos(angle);
    
                vRightRot = vRight * c + vUp * s;
                vLookRot = vUp * c - vRight * s;
    
                vR = vRightRot * (g_fSize.x * In[0].fSize * 0.5f);
                vL = vLookRot * (g_fSize.y * In[0].fSize * 0.5f);
            }
            break;
    }
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    if (4 <= g_iBillboard)
    {
        Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vR + vL * 2, 1.f), matVP);
        Out[0].vTexcoord = float2(0.f, 0.f);
        Out[0].vLifeTime = In[0].vLifeTime;
        Out[0].vProjPos = mul(In[0].vProjPos + float4(vR, 0) + float4(vL, 0) * 2, matVP);
    
        Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vR + vL * 2, 1.f), matVP);
        Out[1].vTexcoord = float2(1.f, 0.f);
        Out[1].vLifeTime = In[0].vLifeTime;
        Out[1].vProjPos = mul(In[0].vProjPos - float4(vR, 0) + float4(vL, 0) * 2, matVP);
    
        Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vR, 1.f), matVP);
        Out[2].vTexcoord = float2(1.f, 1.f);
        Out[2].vLifeTime = In[0].vLifeTime;
        Out[2].vProjPos = mul(In[0].vProjPos - float4(vR, 0), matVP);
    
        Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vR, 1.f), matVP);
        Out[3].vTexcoord = float2(0.f, 1.f);
        Out[3].vLifeTime = In[0].vLifeTime;
        Out[3].vProjPos = mul(In[0].vProjPos + float4(vR, 0), matVP);
    
        vector vSourDir, vDestDir, vNormal;
        vSourDir = float4(In[0].vPosition.xyz - vR + vL, 1.f) - float4(In[0].vPosition.xyz + vR + vL, 1.f);
        vDestDir = float4(In[0].vPosition.xyz - vR - vL, 1.f) - float4(In[0].vPosition.xyz - vR + vL, 1.f);
        vNormal = normalize(float4(cross(vSourDir.xyz, vDestDir.xyz), 0));
        for (int i = 0; i < 4; ++i)
        {
            Out[i].vNormal = float4(normalize(vNormal.xyz), 0);
            Out[i].vTangent = -float4(normalize(vRightRot), 0);
            Out[i].vBitangent = -float4(normalize(vLookRot), 0);
        }
        Out[0].vSeed = In[0].vSeed;
        Out[1].vSeed = In[0].vSeed;
        Out[2].vSeed = In[0].vSeed;
        Out[3].vSeed = In[0].vSeed;
        OutStream.Append(Out[0]);
        OutStream.Append(Out[1]);
        OutStream.Append(Out[2]);
        OutStream.RestartStrip();
    
        OutStream.Append(Out[0]);
        OutStream.Append(Out[2]);
        OutStream.Append(Out[3]);
        OutStream.RestartStrip();
    }
    else
    {
        Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vR + vL, 1.f), matVP);
        Out[0].vTexcoord = float2(0.f, 0.f);
        Out[0].vLifeTime = In[0].vLifeTime;
        Out[0].vProjPos = mul(In[0].vProjPos + float4(vR, 0) + float4(vL, 0), matVP);
    
        Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vR + vL, 1.f), matVP);
        Out[1].vTexcoord = float2(1.f, 0.f);
        Out[1].vLifeTime = In[0].vLifeTime;
        Out[1].vProjPos = mul(In[0].vProjPos - float4(vR, 0) + float4(vL, 0), matVP);
    
        Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vR - vL, 1.f), matVP);
        Out[2].vTexcoord = float2(1.f, 1.f);
        Out[2].vLifeTime = In[0].vLifeTime;
        Out[2].vProjPos = mul(In[0].vProjPos - float4(vR, 0) - float4(vL, 0), matVP);
    
        Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vR - vL, 1.f), matVP);
        Out[3].vTexcoord = float2(0.f, 1.f);
        Out[3].vLifeTime = In[0].vLifeTime;
        Out[3].vProjPos = mul(In[0].vProjPos + float4(vR, 0) - float4(vL, 0), matVP);
    
        vector vSourDir, vDestDir, vNormal;
        vSourDir = float4(In[0].vPosition.xyz - vR + vL, 1.f) - float4(In[0].vPosition.xyz + vR + vL, 1.f);
        vDestDir = float4(In[0].vPosition.xyz - vR - vL, 1.f) - float4(In[0].vPosition.xyz - vR + vL, 1.f);
        vNormal = normalize(float4(cross(vSourDir.xyz, vDestDir.xyz), 0));
        for (int i = 0; i < 4; ++i)
        {
            Out[i].vNormal = float4(normalize(vNormal.xyz), 0);
            Out[i].vTangent = -float4(normalize(vRightRot), 0);
            Out[i].vBitangent = -float4(normalize(vLookRot), 0);
        }
        Out[0].vSeed = In[0].vSeed;
        Out[1].vSeed = In[0].vSeed;
        Out[2].vSeed = In[0].vSeed;
        Out[3].vSeed = In[0].vSeed;
        OutStream.Append(Out[0]);
        OutStream.Append(Out[1]);
        OutStream.Append(Out[2]);
        OutStream.RestartStrip();
    
        OutStream.Append(Out[0]);
        OutStream.Append(Out[2]);
        OutStream.Append(Out[3]);
        OutStream.RestartStrip();
    }
}

[maxvertexcount(6)]
void GS_NONLIGHT_BILLBOARD(point GS_IN In[1], inout TriangleStream<GS_NONLIGHT_OUT> OutStream)
{
    GS_NONLIGHT_OUT Out[4];
    float3 vR;
    float3 vL;
        
    float angle = radians(g_fAngle);
    float s = sin(angle);
    float c = cos(angle);
    switch (g_iBillboard % 4)
    {
        case 0:
            {
                float3 vLook = normalize(In[0].TransformMatrix._31_32_33);
                float3 vRight = normalize(In[0].TransformMatrix._11_12_13);
    
                float3 vRightRot = vRight * c + vLook * s;
                float3 vLookRot = vLook * c - vRight * s;
                if (!g_bisSpectrum)
                {
                    vRightRot = normalize(mul(float4(vRightRot, 0), g_WorldMatrix)).xyz;
                    vLookRot = normalize(mul(float4(vLookRot, 0), g_WorldMatrix)).xyz;
                }
                vR = vRightRot * (g_fSize.x * In[0].fSize * 0.5f);
                vL = vLookRot * (g_fSize.y * In[0].fSize * 0.5f);
                vLook *= g_fSize.x * In[0].fSize;
            }
            break;
        case 1:
            {
                float3 vRight = -normalize(g_CamMatrix._11_12_13);
                float3 vUp = normalize(g_CamMatrix._21_22_23);
    
                float3 vRightRot = vRight * c + vUp * s;
                float3 vLookRot = vUp * c - vRight * s;
    
                vR = vRightRot * (g_fSize.x * In[0].fSize * 0.5f);
                vL = vLookRot * (g_fSize.y * In[0].fSize * 0.5f);
            }
            break;
        case 2:
            {
                float3 vLook = normalize(In[0].TransformMatrix._31_32_33);
                float3 vRight = normalize(float4(cross(normalize(g_CamMatrix._31_32_33), vLook), 0));
        
                float3 vRightRot = vRight * c + vLook * s;
                float3 vLookRot = vLook * c - vRight * s;
                if (!g_bisSpectrum)
                {
                    vRightRot = normalize(mul(float4(vRightRot, 0), g_WorldMatrix)).xyz;
                    vLookRot = normalize(mul(float4(vLookRot, 0), g_WorldMatrix)).xyz;
                }
                vR = vRightRot * (g_fSize.x * In[0].fSize * 0.5f);
                vL = vLookRot * (g_fSize.y * In[0].fSize * 0.5f);
            }
            break;
        case 3:
            {
                float3 vRight = -normalize(g_CamMatrix._11_12_13);
                float3 vUp = normalize(g_CamMatrix._21_22_23);
                float3 localUp = normalize(float3(
    In[0].TransformMatrix._31,
    In[0].TransformMatrix._32,
    In[0].TransformMatrix._33));
        
                float projRight = dot(localUp, vRight);
                float projUp = dot(localUp, vUp);
        
        
        
                float3 up = normalize(mul(float3(In[0].TransformMatrix._31, In[0].TransformMatrix._32, In[0].TransformMatrix._33), g_CamMatrix._31_32_33));
                float angle = atan2(projUp, projRight) + radians(g_fAngle);
    
                float s = sin(angle);
                float c = cos(angle);
    
                float3 vRightRot = vRight * c + vUp * s;
                float3 vLookRot = vUp * c - vRight * s;
    
                vR = vRightRot * (g_fSize.x * In[0].fSize * 0.5f);
                vL = vLookRot * (g_fSize.y * In[0].fSize * 0.5f);
            }
            break;
    }
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    if (4 <= g_iBillboard)
    {
        Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vR + vL * 2, 1.f), matVP);
        Out[0].vTexcoord = float2(0.f, 0.f);
        Out[0].vLifeTime = In[0].vLifeTime;
    
        Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vR + vL * 2, 1.f), matVP);
        Out[1].vTexcoord = float2(1.f, 0.f);
        Out[1].vLifeTime = In[0].vLifeTime;
    
        Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vR, 1.f), matVP);
        Out[2].vTexcoord = float2(1.f, 1.f);
        Out[2].vLifeTime = In[0].vLifeTime;
    
        Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vR, 1.f), matVP);
        Out[3].vTexcoord = float2(0.f, 1.f);
        Out[3].vLifeTime = In[0].vLifeTime;
        Out[0].vSeed = In[0].vSeed;
        Out[1].vSeed = In[0].vSeed;
        Out[2].vSeed = In[0].vSeed;
        Out[3].vSeed = In[0].vSeed;
    
        OutStream.Append(Out[0]);
        OutStream.Append(Out[1]);
        OutStream.Append(Out[2]);
        OutStream.RestartStrip();
    
        OutStream.Append(Out[0]);
        OutStream.Append(Out[2]);
        OutStream.Append(Out[3]);
        OutStream.RestartStrip();
    }
    else
    {
        Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vR + vL, 1.f), matVP);
        Out[0].vTexcoord = float2(0.f, 0.f);
        Out[0].vLifeTime = In[0].vLifeTime;
    
        Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vR + vL, 1.f), matVP);
        Out[1].vTexcoord = float2(1.f, 0.f);
        Out[1].vLifeTime = In[0].vLifeTime;
    
        Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vR - vL, 1.f), matVP);
        Out[2].vTexcoord = float2(1.f, 1.f);
        Out[2].vLifeTime = In[0].vLifeTime;
    
        Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vR - vL, 1.f), matVP);
        Out[3].vTexcoord = float2(0.f, 1.f);
        Out[3].vLifeTime = In[0].vLifeTime;
        Out[0].vSeed = In[0].vSeed;
        Out[1].vSeed = In[0].vSeed;
        Out[2].vSeed = In[0].vSeed;
        Out[3].vSeed = In[0].vSeed;
    
        OutStream.Append(Out[0]);
        OutStream.Append(Out[1]);
        OutStream.Append(Out[2]);
        OutStream.RestartStrip();
    
        OutStream.Append(Out[0]);
        OutStream.Append(Out[2]);
        OutStream.Append(Out[3]);
        OutStream.RestartStrip();
    }
}

[maxvertexcount(6)]
void GS_WEIGHT_BILLBOARD(point GS_IN In[1], inout TriangleStream<GS_WEIGHT_OUT> OutStream)
{
    GS_WEIGHT_OUT Out[4];
    float3 vR;
    float3 vL;
        
    float angle = radians(g_fAngle);
    float s = sin(angle);
    float c = cos(angle);
    switch (g_iBillboard % 4)
    {
        case 0:
            {
                float3 vLook = normalize(In[0].TransformMatrix._31_32_33);
                float3 vRight = normalize(In[0].TransformMatrix._11_12_13);
        
                float3 vRightRot = vRight * c + vLook * s;
                float3 vLookRot = vLook * c - vRight * s;
                if (!g_bisSpectrum)
                {
                    vRightRot = normalize(mul(float4(vRightRot, 0), g_WorldMatrix)).xyz;
                    vLookRot = normalize(mul(float4(vLookRot, 0), g_WorldMatrix)).xyz;
                }
                vR = vRightRot * (g_fSize.x * In[0].fSize * 0.5f);
                vL = vLookRot * (g_fSize.y * In[0].fSize * 0.5f);
            }
            break;
        case 1:
            {
                float3 vRight = -normalize(g_CamMatrix._11_12_13);
                float3 vUp = normalize(g_CamMatrix._21_22_23);
    
    
                float3 vRightRot = normalize(vRight * c + vUp * s);
                float3 vLookRot = normalize(vUp * c - vRight * s);
    
                vR = vRightRot * (g_fSize.x * In[0].fSize * 0.5f);
                vL = vLookRot * (g_fSize.y * In[0].fSize * 0.5f);
            }
            break;
        case 2:
            {
                float3 vLook = normalize(In[0].TransformMatrix._31_32_33);
                float3 vRight = normalize(float4(cross(normalize(g_CamMatrix._31_32_33), vLook), 0));
    
                float3 vRightRot = vRight * c + vLook * s;
                float3 vLookRot = vLook * c - vRight * s;
                if (!g_bisSpectrum)
                {
                    vRightRot = normalize(mul(float4(vRightRot, 0), g_WorldMatrix)).xyz;
                    vLookRot = normalize(mul(float4(vLookRot, 0), g_WorldMatrix)).xyz;
                }
                vR = vRightRot * (g_fSize.x * In[0].fSize * 0.5f);
                vL = vLookRot * (g_fSize.y * In[0].fSize * 0.5f);
            }
            break;
        case 3:
            {
                float3 vRight = -normalize(g_CamMatrix._11_12_13);
                float3 vUp = normalize(g_CamMatrix._21_22_23);
                float3 localUp = normalize(float3(
    In[0].TransformMatrix._31,
    In[0].TransformMatrix._32,
    In[0].TransformMatrix._33));
        
                float projRight = dot(localUp, vRight);
                float projUp = dot(localUp, vUp);
        
        
        
                float3 up = normalize(mul(float3(In[0].TransformMatrix._31, In[0].TransformMatrix._32, In[0].TransformMatrix._33), g_CamMatrix._31_32_33));
                float angle = atan2(projUp, projRight) + radians(g_fAngle);
    
                float s = sin(angle);
                float c = cos(angle);
    
                float3 vRightRot = vRight * c + vUp * s;
                float3 vLookRot = vUp * c - vRight * s;
    
                vR = vRightRot * (g_fSize.x * In[0].fSize * 0.5f);
                vL = vLookRot * (g_fSize.y * In[0].fSize * 0.5f);
            }
            break;
    }
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    if (4 <= g_iBillboard)
    {
        Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vR + vL * 2, 1.f), matVP);
        Out[0].vTexcoord = float2(0.f, 0.f);
        Out[0].vLifeTime = In[0].vLifeTime;
        Out[0].vProjPos = mul(In[0].vProjPos + float4(vR, 0) + float4(vL, 0) * 2, matVP);
    
        Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vR + vL * 2, 1.f), matVP);
        Out[1].vTexcoord = float2(1.f, 0.f);
        Out[1].vLifeTime = In[0].vLifeTime;
        Out[1].vProjPos = mul(In[0].vProjPos - float4(vR, 0) + float4(vL, 0) * 2, matVP);
    
        Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vR, 1.f), matVP);
        Out[2].vTexcoord = float2(1.f, 1.f);
        Out[2].vLifeTime = In[0].vLifeTime;
        Out[2].vProjPos = mul(In[0].vProjPos - float4(vR, 0), matVP);
                                                              
        Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vR, 1.f), matVP);
        Out[3].vTexcoord = float2(0.f, 1.f);
        Out[3].vLifeTime = In[0].vLifeTime;
        Out[3].vProjPos = mul(In[0].vProjPos + float4(vR, 0), matVP);
        Out[0].vSeed = In[0].vSeed;
        Out[1].vSeed = In[0].vSeed;
        Out[2].vSeed = In[0].vSeed;
        Out[3].vSeed = In[0].vSeed;
    
        OutStream.Append(Out[0]);
        OutStream.Append(Out[1]);
        OutStream.Append(Out[2]);
        OutStream.RestartStrip();
    
        OutStream.Append(Out[0]);
        OutStream.Append(Out[2]);
        OutStream.Append(Out[3]);
        OutStream.RestartStrip();
    }
    else
    {
        Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vR + vL, 1.f), matVP);
        Out[0].vTexcoord = float2(0.f, 0.f);
        Out[0].vLifeTime = In[0].vLifeTime;
        Out[0].vProjPos = mul(In[0].vProjPos + float4(vR, 0) + float4(vL, 0), matVP);
    
        Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vR + vL, 1.f), matVP);
        Out[1].vTexcoord = float2(1.f, 0.f);
        Out[1].vLifeTime = In[0].vLifeTime;
        Out[1].vProjPos = mul(In[0].vProjPos - float4(vR, 0) + float4(vL, 0), matVP);
    
        Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vR - vL, 1.f), matVP);
        Out[2].vTexcoord = float2(1.f, 1.f);
        Out[2].vLifeTime = In[0].vLifeTime;
        Out[2].vProjPos = mul(In[0].vProjPos - float4(vR, 0) - float4(vL, 0), matVP);
    
        Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vR - vL, 1.f), matVP);
        Out[3].vTexcoord = float2(0.f, 1.f);
        Out[3].vLifeTime = In[0].vLifeTime;
        Out[3].vProjPos = mul(In[0].vProjPos + float4(vR, 0) - float4(vL, 0), matVP);
        Out[0].vSeed = In[0].vSeed;
        Out[1].vSeed = In[0].vSeed;
        Out[2].vSeed = In[0].vSeed;
        Out[3].vSeed = In[0].vSeed;
    
        OutStream.Append(Out[0]);
        OutStream.Append(Out[1]);
        OutStream.Append(Out[2]);
        OutStream.RestartStrip();
    
        OutStream.Append(Out[0]);
        OutStream.Append(Out[2]);
        OutStream.Append(Out[3]);
        OutStream.RestartStrip();
    }
}

[maxvertexcount(6)]
void GS_BILLBOARD_ROTATION(point GS_IN In[1], inout TriangleStream<GS_WEIGHT_OUT> OutStream)
{
    GS_WEIGHT_OUT Out[4];
    float3 vR;
    float3 vL;
        float t = In[0].vLifeTime.x / In[0].vLifeTime.y;
        float fSize = (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(0)) * 100
     + (-2 * pow(t, 3) + 3 * pow(t, 2)) * 180
     + (pow(t, 3) - pow(t, 2)) * tan(radians(0)) * 100;
    
    float angle = radians(g_fAngle + fSize);
    float s = sin(angle);
    float c = cos(angle);
    float3 vRight = -normalize(g_CamMatrix._11_12_13);
    float3 vUp = normalize(g_CamMatrix._21_22_23);
    
    float3 vRightRot = vRight * c + vUp * s;
    float3 vLookRot = vUp * c - vRight * s;
    
    vR = vRightRot * (g_fSize.x * In[0].fSize * 0.5f);
    vL = vLookRot * (g_fSize.y * In[0].fSize * 0.5f);
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vR + vL, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[3].vProjPos = mul(In[0].vProjPos + float4(vR, 0) + float4(vL, 0), matVP);
    
    Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vR + vL, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[3].vProjPos = mul(In[0].vProjPos - float4(vR, 0) + float4(vL, 0), matVP);
    
    Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vR - vL, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[3].vProjPos = mul(In[0].vProjPos - float4(vR, 0) - float4(vL, 0), matVP);
    
    Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vR - vL, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vProjPos = mul(In[0].vProjPos + float4(vR, 0) - float4(vL, 0), matVP);
    Out[0].vSeed = In[0].vSeed;
    Out[1].vSeed = In[0].vSeed;
    Out[2].vSeed = In[0].vSeed;
    Out[3].vSeed = In[0].vSeed;
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}

/* 출력된 정점 위치벡터의 w값으로 모든 성분을 나눈다 -> 투영스페이스로 변환 */ 
/* 정점의 위치에 대해서 뷰포트 변환을 수행한다 */ 
/* 정점의 모든 정보를 보간하여 픽셀을 만든다. -> 래스터라이즈 */ 

struct PS_NORMAL_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vNormal : TEXCOORD3;
    float4 vTangent : TEXCOORD4;
    float4 vBitangent : TEXCOORD5;
    int vSeed : SEED;
};


struct PS_NONLIGHT_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    int vSeed : SEED;
};

struct PS_WEIGHT_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    int vSeed : SEED;
};

struct PS_NORMAL_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vOrm : SV_TARGET3;
};

struct PS_NONLIGHT_OUT
{
    float4 vDiffuse : SV_TARGET0;
};

struct PS_BLOOM_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vBloom : SV_TARGET1;
};

float hole[16] =
{
    0, 8, 2, 10,
    12, 4, 14, 6,
     3, 11, 1, 9,
    15, 7, 13, 5
};

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NORMAL_OUT PS_NORMAL(PS_NORMAL_IN In, bool isFrontFace : SV_IsFrontFace)
{
    PS_NORMAL_OUT Out;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.rgb *= fmod(frac(sin(In.vSeed * 0.01) * 123456.789012), 0.6) + 0.4;
    Out.vDiffuse *= g_MaskTexture.Sample(DefaultSampler, fTexcoord).r * saturate((In.vLifeTime.y - In.vLifeTime.x) * 20);
    
    int index = (int(In.vPosition.x) & 3) + (int(In.vPosition.y) & 3) * 4;
    
    float threshold = hole[index] / 32.0;
    if (0 >= Out.vDiffuse.a - threshold || 0 >= In.vLifeTime.y - In.vLifeTime.x)
        discard;
    float2 rg = g_NormalTexture.Sample(DefaultSampler, fTexcoord).xy * 2.f - 1.f;
    float3 normal;
    normal.xy = rg;
    normal.z = sqrt(saturate(1.0 - dot(rg, rg)));
    normal = normalize(normal);
    
    float3 N = normal;
    float3 T = normalize(In.vTangent.xyz);
    float3 B = normalize(In.vBitangent.xyz);
    float3 G = normalize(In.vNormal.xyz);
    
    if (!isFrontFace)
    {
        G *= -1;
    }
    
    float3x3 TBN = float3x3(T, B, G);
    float3 finalNormal = normalize(mul(N, TBN));
    Out.vNormal = float4(finalNormal * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    Out.vOrm = float4(0, 0, 0, 0);
    
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NORMAL_OUT PS_NORMAL_DIFFUSE(PS_NORMAL_IN In, bool isFrontFace : SV_IsFrontFace)
{
    PS_NORMAL_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + In.vLifeTime.x * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + In.vLifeTime.x * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + In.vLifeTime.x * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + In.vLifeTime.x * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, fTexcoord);
    if (0.5 > Out.vDiffuse.a)
        discard;
    Out.vDiffuse *= g_vColor;
    Out.vDiffuse.rgb *= fmod(frac(sin(In.vSeed * 0.01) * 123456.789012), 0.6) + 0.4;
    Out.vDiffuse.a *= saturate((In.vLifeTime.y - In.vLifeTime.x) * 20);
    
    int index = (int(In.vPosition.x) & 3) + (int(In.vPosition.y) & 3) * 4;
    
    float threshold = hole[index] / 32.0;
    if (0 >= Out.vDiffuse.a - threshold || 0 >= In.vLifeTime.y - In.vLifeTime.x)
        discard;
    float2 rg = g_NormalTexture.Sample(DefaultSampler, fTexcoord).xy * 2.f - 1.f;
    float3 normal;
    normal.xy = rg;
    normal.z = sqrt(saturate(1.0 - dot(rg, rg)));
    normal = normalize(normal);
    
    float3 N = normal;
    float3 T = normalize(In.vTangent.xyz);
    float3 B = normalize(In.vBitangent.xyz);
    float3 G = normalize(In.vNormal.xyz);
    
    if (!isFrontFace)
    {
        //T *= -1;
        //B *= -1;
        G *= -1;
    }
    
    float3x3 TBN = float3x3(T, B, G);
    float3 finalNormal = normalize(mul(N, TBN));
    Out.vNormal = float4(finalNormal * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    Out.vOrm = float4(0, 0, 0, 0);
    
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_BLOOM_OUT PS_BLOOM(PS_NONLIGHT_IN In)
{
    PS_BLOOM_OUT Out;
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    

    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    Out.vDiffuse = float4(0, 0, 0, 0);
    Out.vBloom = g_vColor;
    Out.vBloom *= g_vColor.a * min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a); // * saturate(In.vLifeTime.y - In.vLifeTime.x);
    if (0 >= Out.vBloom.a)
        discard;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_NONLIGHT(PS_NONLIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    

    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = g_vColor * min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a) * saturate(In.vLifeTime.y - In.vLifeTime.x);
    Out.vDiffuse *= g_vColor.a;
    if (0 >= Out.vDiffuse.a)
        discard;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_BLACK_BLEND(PS_WEIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    if (In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a) * saturate(In.vLifeTime.y - In.vLifeTime.x);
    if (0 >= Out.vDiffuse.a)
        discard;
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = saturate(exp(-linearDepth * 20));
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight * g_fDissolveUVSize.x;
    Out.vDiffuse.a = Out.vDiffuse.a * weight * g_fDissolveUVSize.x;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_BLACK_DEPTH(PS_NONLIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    

    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = g_vColor * min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a) * saturate(In.vLifeTime.y - In.vLifeTime.x);
    Out.vDiffuse *= g_vColor.a;
    //if (0 >= Out.vDiffuse.a)
    discard;
    Out.vDiffuse = 0;
    return Out;
}


/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_GLOW(PS_WEIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a) * saturate((In.vLifeTime.y - In.vLifeTime.x) * g_fDissolveUVSpeed.x);
    //Out.vDiffuse.a = saturate(pow(Out.vDiffuse.a * 2, 2));
    
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = lerp(0.3, 0.5, saturate(exp(-linearDepth * 20)));
    if (weight > Out.vDiffuse.a)
        discard;
    Out.vDiffuse.a = 1;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_GLOW_BLOOM(PS_NONLIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a) * saturate((In.vLifeTime.y - In.vLifeTime.x) * g_fDissolveUVSpeed.x);
    if (0 >= Out.vDiffuse.a)
        discard;
    Out.vDiffuse.rgb *= Out.vDiffuse.a;
    Out.vDiffuse.a = 1;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_BLUR(PS_WEIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    float2 uv = In.vPosition.xy / float2(1600, 900);
    if (In.vProjPos.z / In.vProjPos.w > g_DepthTexture.Sample(DefaultSampler, uv * 2).r || In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a) * saturate(In.vLifeTime.y - In.vLifeTime.x);
    if (0 >= Out.vDiffuse.a)
        discard;
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = saturate(exp(-linearDepth * 10));
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight * g_fDissolveUVSize.x;
    Out.vDiffuse.a = Out.vDiffuse.a * weight * g_fDissolveUVSize.x;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_METABALL(PS_WEIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    float2 uv = In.vPosition.xy / float2(1600, 900);
    if (In.vProjPos.z / In.vProjPos.w > g_DepthTexture.Sample(DefaultSampler, uv * 2).r || In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a) * saturate(In.vLifeTime.y - In.vLifeTime.x);
    if (0 >= Out.vDiffuse.a)
        discard;
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = saturate(exp(-linearDepth * 10));
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight;
    Out.vDiffuse.a = Out.vDiffuse.a * weight;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_ELECTRIC(PS_WEIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + In.vLifeTime.x * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + In.vLifeTime.x * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 > In.vLifeTime.x)
        discard;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, -In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    float2 distort = (g_DiffuseTexture.Sample(MirrorSampler, DiffuseTexcoord).rb * 2.0 - 1.0) * 0.03 * g_fDissolveUVSize.y;
    
    float2 uvDistorted = fTexcoord + distort * pow((1 - In.vTexcoord.y), 2);
    
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, uvDistorted).r, g_MaskTexture.Sample(DefaultSampler, uvDistorted).a);
    
    Out.vDiffuse *= pow(saturate(((In.vLifeTime.x / In.vLifeTime.y)) * 2 / (1 - In.vTexcoord.y)), 2) * pow(saturate((1 - In.vTexcoord.y) / ((In.vLifeTime.x / In.vLifeTime.y) * 2)), 5);
    
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = lerp(0.3, 0.5, saturate(exp(-linearDepth * 20)));
    if (weight > Out.vDiffuse.a)
        discard;
    Out.vDiffuse.a = 1;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_ELECTRIC_BLOOM(PS_NONLIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    //discard;
    if (In.vLifeTime.y < In.vLifeTime.x || 0 > In.vLifeTime.x)
        discard;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + In.vLifeTime.x * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + In.vLifeTime.x * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    
    
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, -In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    float2 distort = (g_DiffuseTexture.Sample(MirrorSampler, DiffuseTexcoord).rb * 2.0 - 1.0) * 0.03 * g_fDissolveUVSize.y;
    
    float2 uvDistorted = fTexcoord + distort * pow((1 - In.vTexcoord.y), 2);
    
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, uvDistorted).r, g_MaskTexture.Sample(DefaultSampler, uvDistorted).a);
    Out.vDiffuse *= pow(saturate(((In.vLifeTime.x / In.vLifeTime.y)) * 2 / (1 - In.vTexcoord.y)), 5) * pow(saturate((1 - In.vTexcoord.y) / ((In.vLifeTime.x / In.vLifeTime.y) * 2)), 15);
    if (0 >= Out.vDiffuse.a)
        discard;
    Out.vDiffuse.rgb *= Out.vDiffuse.a;
    Out.vDiffuse.a = 1;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_REVERSE_ELECTRIC(PS_WEIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + In.vLifeTime.x * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + In.vLifeTime.x * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 > In.vLifeTime.x)
        discard;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, -In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    float2 distort = (g_DiffuseTexture.Sample(MirrorSampler, DiffuseTexcoord).rb * 2.0 - 1.0) * 0.03 * g_fDissolveUVSize.y;
    
    float2 uvDistorted = fTexcoord + distort * pow(In.vTexcoord.y, 2);
    
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, uvDistorted).r, g_MaskTexture.Sample(DefaultSampler, uvDistorted).a);
    
    Out.vDiffuse *= pow(saturate((1 - (In.vLifeTime.x / In.vLifeTime.y)) * 2 / (1 - In.vTexcoord.y)), 2) * pow(saturate((1 - In.vTexcoord.y) / ((1 - (In.vLifeTime.x / In.vLifeTime.y)) * 2)), 5);
    
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = lerp(0.3, 0.5, saturate(exp(-linearDepth * 20)));
    if (weight > Out.vDiffuse.a)
        discard;
    Out.vDiffuse.a = 1;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_REVERSE_ELECTRIC_BLOOM(PS_NONLIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    //discard;
    if (In.vLifeTime.y < In.vLifeTime.x || 0 > In.vLifeTime.x)
        discard;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + In.vLifeTime.x * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + In.vLifeTime.x * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    
    
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, -In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    float2 distort = (g_DiffuseTexture.Sample(MirrorSampler, DiffuseTexcoord).rb * 2.0 - 1.0) * 0.03 * g_fDissolveUVSize.y;
    
    float2 uvDistorted = fTexcoord + distort * pow(In.vTexcoord.y, 2);
    
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, uvDistorted).r, g_MaskTexture.Sample(DefaultSampler, uvDistorted).a);
    Out.vDiffuse *= pow(saturate((1 - (In.vLifeTime.x / In.vLifeTime.y)) * 2 / (1 - In.vTexcoord.y)), 5) * pow(saturate((1 - In.vTexcoord.y) / ((1 - (In.vLifeTime.x / In.vLifeTime.y)) * 2)), 15);
    if (0 >= Out.vDiffuse.a)
        discard;
    Out.vDiffuse.rgb *= Out.vDiffuse.a;
    Out.vDiffuse.a = 1;
    return Out;
}

PS_NONLIGHT_OUT PS_NOISE_DISTORTION(PS_NONLIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + In.vLifeTime.x * g_fDiffuseUVSpeed.x / 180) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + In.vLifeTime.x * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + In.vLifeTime.x * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + In.vLifeTime.x * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    float2 tex = In.vTexcoord;
    float s = sin(radians(In.vLifeTime.x * -g_fDiffuseUVSpeed.x));
    float c = cos(radians(In.vLifeTime.x * -g_fDiffuseUVSpeed.x));
    tex -= float2(0.5, 0.5);
    float2x2 rot = float2x2(c, -s, s, c);
    tex = mul(tex, rot) + float2(0.5, 0.5);
    
    float2 uv = In.vTexcoord;
    
    float dist = distance(uv, float2(0.5, 0.5));
    
    float ring = smoothstep(saturate((In.vLifeTime.x + 2) / In.vLifeTime.y), saturate((In.vLifeTime.x + 2) / In.vLifeTime.y) - 0.1, dist);
    
    float3 noise = g_DiffuseTexture.Sample(DefaultSampler, tex).rgb;
    
    float2 distort = (noise.rb * 2.0 - 1.0) * 0.2;
    
    float2 uvDistorted = uv + distort * ring;
    
    float4 col = g_NormalTexture.Sample(DefaultSampler, uvDistorted);
    
    float mask = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
    col.rgb *= ring * mask;
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= col.r * saturate(In.vLifeTime.y - In.vLifeTime.x);
    if (0 >= Out.vDiffuse.a)
        discard;
    Out.vDiffuse.rgb *= Out.vDiffuse.a;
    return Out;
}

PS_NONLIGHT_OUT PS_SIGNAL_SPHERE(PS_WEIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + In.vLifeTime.x * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + In.vLifeTime.x * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + In.vLifeTime.x * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + In.vLifeTime.x * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    float diffuse = (g_DiffuseTexture.Sample(MirrorSampler, DiffuseTexcoord).r - 0.5) * 2 * saturate((In.vLifeTime.y - In.vLifeTime.x));
    Out.vDiffuse = g_vColor;
    float2 texcoord = float2(MaskTexcoord.x * 1.2 + diffuse * 0.2 - 0.2 * 0.5, MaskTexcoord.y * 1.2 + diffuse * 0.2 - 0.2 * 0.5);
    Out.vDiffuse.a *= length(float2(0.5, 0.5) - texcoord) * 2;
    Out.vDiffuse.a = pow(Out.vDiffuse.a, 10);
    Out.vDiffuse.a *= saturate((In.vLifeTime.y - In.vLifeTime.x) * 1.5);
    if (0 > Out.vDiffuse.a || 0.5 < length(float2(0.5, 0.5) - texcoord))
        discard;
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = saturate(exp(-linearDepth * 20));
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight * g_fDissolveUVSize.x;
    Out.vDiffuse.a = Out.vDiffuse.a * weight * g_fDissolveUVSize.x;
    return Out;
}

PS_NONLIGHT_OUT PS_PLASMA(PS_WEIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + In.vLifeTime.x * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + In.vLifeTime.x * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + In.vLifeTime.x * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + In.vLifeTime.x * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    
    Out.vDiffuse = g_vColor;
    if (In.vTexcoord.y <= 0.3)
    {
        float a = (((1 - (In.vTexcoord.y * (1 / 0.3))) * 4) + 1);
        MaskTexcoord.x = In.vTexcoord.x * a;
        MaskTexcoord.x -= (a - 1) * 0.5;
        Out.vDiffuse.a /= pow(a, 2);
    }
    if (In.vTexcoord.y >= 0.7)
    {
        float a = (((1 - ((1 - In.vTexcoord.y) * (1 / 0.3))) * 4) + 1);
        MaskTexcoord.x = In.vTexcoord.x * a;
        MaskTexcoord.x -= (a - 1) * 0.5;
        Out.vDiffuse.a /= pow(a, 2);

    }
    MaskTexcoord.x = MaskTexcoord.x * (In.vLifeTime.x * 40 + 1);
    MaskTexcoord.x -= ((In.vLifeTime.x * 40 + 1) - 1) * 0.5;
    
    if (0 <= MaskTexcoord.y)
        MaskTexcoord.y = fmod(MaskTexcoord.y, 1.f);
    else
    {
        MaskTexcoord.y = 1 - fmod(abs(MaskTexcoord.y), 1.f);
    }
    
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(NoneSampler, MaskTexcoord).r, g_MaskTexture.Sample(NoneSampler, MaskTexcoord).a) * saturate((In.vLifeTime.y - In.vLifeTime.x) * 3);
    if (0 >= Out.vDiffuse.a)
        discard;
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = saturate(exp(-linearDepth * 20));
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight * g_fDissolveUVSize.x;
    Out.vDiffuse.a = Out.vDiffuse.a * weight * g_fDissolveUVSize.x;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_SIGNAL_POWER(PS_WEIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + In.vLifeTime.x * g_fDiffuseUVSpeed.x / 180) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + In.vLifeTime.x * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + In.vLifeTime.x * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + In.vLifeTime.x * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    float2 tex = In.vTexcoord;
    float s = sin(radians(In.vLifeTime.x * -g_fDiffuseUVSpeed.x));
    float c = cos(radians(In.vLifeTime.x * -g_fDiffuseUVSpeed.x));
    tex -= float2(0.5, 0.5);
    float2x2 rot = float2x2(c, -s, s, c);
    tex = mul(tex, rot) + float2(0.5, 0.5);
    
    float2 uv = In.vTexcoord;
    
    float dist = distance(uv, float2(0.5, 0.5));
    
    float ring = smoothstep(saturate((In.vLifeTime.x + 2) / In.vLifeTime.y), saturate((In.vLifeTime.x + 2) / In.vLifeTime.y) - 0.1, dist);
    
    float3 noise = g_DiffuseTexture.Sample(DefaultSampler, tex).rgb;
    
    float2 distort = (noise.rb * 2.0 - 1.0) * 0.2;
    
    float2 uvDistorted = uv + distort * ring;
    
    float4 col = g_NormalTexture.Sample(DefaultSampler, uvDistorted);
    
    float mask = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
    col.rgb *= ring * mask;
    //col.a *= g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r;
    Out.vDiffuse = g_vColor;
    
    float diffuse = (g_DiffuseTexture.Sample(MirrorSampler, tex).r - 0.5) * 2 * saturate((In.vLifeTime.y - In.vLifeTime.x));
    Out.vDiffuse = lerp(g_vColor, float4(0.05, 0.1, 1, 1), pow(saturate(In.vLifeTime.x / (In.vLifeTime.y - 0.5)), 3));
    
    //Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x + diffuse * 0.05, MaskTexcoord.y + diffuse * 0.05)).r * saturate(In.vLifeTime.y - In.vLifeTime.x);
    float2 texcoord = float2(MaskTexcoord.x * 1.2 + diffuse * 0.05 - 0.2 * 0.5, MaskTexcoord.y * 1.2 + diffuse * 0.05 - 0.2 * 0.5);
    
    
    Out.vDiffuse.a *= length(float2(0.5, 0.5) - texcoord) * 2;
    Out.vDiffuse.a = pow(Out.vDiffuse.a, 10);
    Out.vDiffuse.a *= saturate((In.vLifeTime.y - In.vLifeTime.x) * 1.5);
    if (0 >= Out.vDiffuse.a || 0.5 < length(float2(0.5, 0.5) - texcoord))
        discard;
    Out.vDiffuse.a *= col.r;
    if (0 >= Out.vDiffuse.a)
        discard;
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = saturate(exp(-linearDepth * 20));
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight * g_fDissolveUVSize.x;
    Out.vDiffuse.a = Out.vDiffuse.a * weight * g_fDissolveUVSize.x;
    return Out;
}

PS_NONLIGHT_OUT PS_BLUE_PLASMA(PS_WEIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);

    Out.vDiffuse = lerp(g_vColor, float4(0, 0.1, 1, g_vColor.a), pow(saturate(In.vLifeTime.x / (In.vLifeTime.y - 0.4)), 3));
    if (In.vTexcoord.y <= 0.3 + ((1 - saturate((In.vLifeTime.y - In.vLifeTime.x) * 3)) * 0.2))
    {
        float a = (((1 - (In.vTexcoord.y * (1 / (0.3 + ((1 - saturate((In.vLifeTime.y - In.vLifeTime.x) * 3)) * 0.2))))) * 4) + 1);
        MaskTexcoord.x = In.vTexcoord.x * a;
        MaskTexcoord.x -= (a - 1) * 0.5;
        Out.vDiffuse.a /= pow(a, 2);
    }
    if (In.vTexcoord.y >= 0.7 - ((1 - saturate((In.vLifeTime.y - In.vLifeTime.x) * 3)) * 0.2))
    {
        float a = (((1 - ((1 - In.vTexcoord.y) * (1 / (0.3 + ((1 - saturate((In.vLifeTime.y - In.vLifeTime.x) * 3)) * 0.2))))) * 4) + 1);
        MaskTexcoord.x = In.vTexcoord.x * a;
        MaskTexcoord.x -= (a - 1) * 0.5;
        Out.vDiffuse.a /= pow(a, 2);

    }
    MaskTexcoord.x = MaskTexcoord.x * (In.vLifeTime.x * 15 + 1);
    MaskTexcoord.x -= ((In.vLifeTime.x * 15 + 1) - 1) * 0.5;
    
    if (0 <= MaskTexcoord.y)
        MaskTexcoord.y = fmod(MaskTexcoord.y, 1.f);
    else
    {
        MaskTexcoord.y = 1 - fmod(abs(MaskTexcoord.y), 1.f);
    }
    
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(NoneSampler, MaskTexcoord).r, g_MaskTexture.Sample(NoneSampler, MaskTexcoord).a) * saturate((In.vLifeTime.y - In.vLifeTime.x) * 3);
    if (0 >= Out.vDiffuse.a)
        discard;
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = saturate(exp(-linearDepth * 20));
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight * 6;
    Out.vDiffuse.a = Out.vDiffuse.a * weight;
    return Out;
}

PS_NONLIGHT_OUT PS_BLUEMASK(PS_WEIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
    int iU = (In.vLifeTime.x / fFPS);
    int iV = In.vLifeTime.x / fFPS / g_iUV.x;
    
    

    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = lerp(g_vColor, float4(0, 0.1, 1, g_vColor.a * 2), pow(saturate(In.vLifeTime.x / (In.vLifeTime.y - 1.8)), 2));
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a) * saturate(In.vLifeTime.y - In.vLifeTime.x * 1.2);
    if (0 >= Out.vDiffuse.a)
        discard;
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = saturate(exp(-linearDepth * 20));
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight * 6;
    Out.vDiffuse.a = Out.vDiffuse.a * weight;
    return Out;
}

PS_NONLIGHT_OUT PS_GLOW_NULL(PS_NONLIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    discard;
    return Out;
}
/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_ROTATION_GLOW(PS_WEIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a) * saturate((In.vLifeTime.y - In.vLifeTime.x) * g_fDissolveUVSpeed.x);
    //Out.vDiffuse.a = saturate(pow(Out.vDiffuse.a * 2, 2));
    
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = lerp(0.3, 0.5, saturate(exp(-linearDepth * 20)));
    if (0.1 >= Out.vDiffuse.a)
        discard;
    Out.vDiffuse.a = 1;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_ROTATION_GLOW_BLOOM(PS_WEIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    int iU;
    int iV;
    if (g_bisAnimation)
    {
        float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
        iU = (In.vLifeTime.x / fFPS);
        iV = In.vLifeTime.x / fFPS / g_iUV.x;
    }
    else
    {
        int i = In.vSeed % (g_iUV.x * g_iUV.y);
        iU = i % g_iUV.x;
        iV = i / g_iUV.x;
    }
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a) * saturate((In.vLifeTime.y - In.vLifeTime.x) * g_fDissolveUVSpeed.x);
    if (0 >= Out.vDiffuse.a)
        discard;
    Out.vDiffuse.rgb *= Out.vDiffuse.a;
    Out.vDiffuse.a = 1;
    return Out;
}

BlendState BS_Dust
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
BlendState BS_AlphaAdd
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = add;
    SrcBlendAlpha = one;
    DestBlendAlpha = one;
    BlendOpAlpha = max;
};

technique11 DefaultTechnique
{
    // idx 0
    pass NonBlend
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NORMAL_BILLBOARD();
        PixelShader = compile ps_5_0 PS_NORMAL();
    }
    // idx 1
    pass NonBlend_Diffuse
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NORMAL_BILLBOARD();
        PixelShader = compile ps_5_0 PS_NORMAL_DIFFUSE();
    }
    // idx 2
    pass Bloom
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_BLOOM();
    }
    // idx 3
    pass DSS_Bloom
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_BLOOM();
    }
    // idx 4
    pass NonLight
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_NONLIGHT();
    }
    // idx 5
    pass DSS_NonLight
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_NONLIGHT();
    }
    // idx 6
    pass Black_Blend
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_BlendAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_BLACK_BLEND();
    }
    // idx 7
    pass Blur
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Dust, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_BLUR();
    }
    // idx 8
    pass Glow
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_GLOW();
    }
    // idx 9
    pass Glow_Bloom
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_BlendAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_GLOW_BLOOM();
    }
    // idx 10
    pass Metaball
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_BlendAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_METABALL();
    }
    // idx 11
    pass Electric
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_ELECTRIC();
    }
    // idx 12
    pass Electric_Bloom
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_BlendAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_ELECTRIC_BLOOM();
    }
    // idx 13
    pass Reverse_Electric
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_REVERSE_ELECTRIC();
    }
    // idx 14
    pass Reverse_Electric_Bloom
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_BlendAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_REVERSE_ELECTRIC_BLOOM();
    }
    // idx 15
    pass Noise_Distortion
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_NOISE_DISTORTION();
    }
    // idx 16
    pass Signal_Sphere
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaAdd, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_SIGNAL_SPHERE();
    }
    // idx 17
    pass Plasma
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaAdd, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_PLASMA();
    }
    // idx 18
    pass Signal_Power
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaAdd, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_SIGNAL_POWER();
    }
    // idx 19
    pass Signal_Plasma
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaAdd, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_BLUE_PLASMA();
    }
    // idx 20
    pass Blue_Light
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaAdd, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_BLUEMASK();
    }
    // idx 21
    pass Glow_Null
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_GLOW_NULL();
    }
    // idx 22
    pass DSS_Glow_Bloom
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_BlendAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_GLOW_BLOOM();
    }
    // idx 23
    pass Glow_Rotation
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_BlendAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_BILLBOARD_ROTATION();
        PixelShader = compile ps_5_0 PS_ROTATION_GLOW();
    }
    // idx 24
    pass Glow_Bloom_Rotation
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_BlendAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_BILLBOARD_ROTATION();
        PixelShader = compile ps_5_0 PS_ROTATION_GLOW_BLOOM();
    }
    // idx 25
    pass DSS_Noise_Distortion
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_BlendAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_NOISE_DISTORTION();
    }
}