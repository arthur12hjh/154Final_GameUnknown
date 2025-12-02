#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_CamMatrix;

texture2D g_MaskTexture, g_DiffuseTexture, g_DissolveTexture;
vector g_vColor = vector(1.f, 1.f, 1.f, 1.f);
vector g_vSize = vector(1.f, 0.f, 1.f, 0.f);
float2 g_fMaskUV = float2(0, 0);
float2 g_fMaskUVSpeed = float2(0, 0);
float2 g_fMaskUVSize = float2(1, 1);
float2 g_fDiffuseUV = float2(0, 0);
float2 g_fDiffuseUVSpeed = float2(0, 0);
float2 g_fDiffuseUVSize = float2(1, 1);
float2 g_fDissolveUV = float2(0, 0);
float2 g_fDissolveUVSpeed = float2(0, 0);
float2 g_fDissolveUVSize = float2(1, 1);
int g_iSizeCount;
bool g_bisBillboard, g_bisSpectrum;
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
    float  fSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
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
    int vSeed : SEED;
};

struct GS_NORMAL_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vNormal : TEXCOORD3;
};

struct GS_NONLIGHT_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
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
    if (g_bisBillboard)
    {
        float3 vRight = -normalize(g_CamMatrix._11_12_13) * In[0].fSize * 0.5f;
        float3 vUp = normalize(g_CamMatrix._21_22_23) * In[0].fSize * 0.5f;
        
        matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
        
        Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
        Out[0].vTexcoord = float2(0.f, 0.f);
        Out[0].vLifeTime = In[0].vLifeTime;
        Out[0].vProjPos = Out[0].vPosition;
        
        Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
        Out[1].vTexcoord = float2(1.f, 0.f);
        Out[1].vLifeTime = In[0].vLifeTime;
        Out[1].vProjPos = Out[1].vPosition;
     
        Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
        Out[2].vTexcoord = float2(1.f, 1.f);
        Out[2].vLifeTime = In[0].vLifeTime;
        Out[2].vProjPos = Out[2].vPosition;
     
        Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
        Out[3].vTexcoord = float2(0.f, 1.f);
        Out[3].vLifeTime = In[0].vLifeTime;
        Out[3].vProjPos = Out[3].vPosition;
        Out[0].vNormal = float4(0, 0, 0, 0);
        Out[1].vNormal = float4(0, 0, 0, 0);
        Out[2].vNormal = float4(0, 0, 0, 0);
        Out[3].vNormal = float4(0, 0, 0, 0);
    
        vector vSourDir, vDestDir, vNormal;
        vSourDir = float4(In[0].vPosition.xyz - vRight * 3 + vUp, 1.f) - float4(In[0].vPosition.xyz + vRight * 3 + vUp, 1.f);
        vDestDir = float4(In[0].vPosition.xyz - vRight * 3 - vUp, 1.f) - float4(In[0].vPosition.xyz - vRight * 3 + vUp, 1.f);
        vNormal = normalize(float4(cross(vSourDir.xyz, vDestDir.xyz), 0));
    
        Out[0].vNormal += vNormal;
        Out[1].vNormal += vNormal;
        Out[2].vNormal += vNormal;
    
        vSourDir = float4(In[0].vPosition.xyz - vRight * 3 - vUp, 1.f) - float4(In[0].vPosition.xyz + vRight * 3 + vUp, 1.f);
        vDestDir = float4(In[0].vPosition.xyz + vRight * 3 - vUp, 1.f) - float4(In[0].vPosition.xyz - vRight * 3 - vUp, 1.f);
        vNormal = normalize(float4(cross(vSourDir.xyz, vDestDir.xyz), 0));
    
        Out[0].vNormal += vNormal;
        Out[2].vNormal += vNormal;
        Out[3].vNormal += vNormal;
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
        float3 vLook = normalize(In[0].TransformMatrix._31_32_33);
        float3 vRight = normalize(In[0].TransformMatrix._11_12_13) * In[0].fSize * 0.5f;
        float3 vUp = normalize(cross(vLook, vRight)) * In[0].fSize * 0.5f;
        matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
        
        
        if (!g_bisSpectrum)
        {
            vRight = normalize(mul(float4(vRight, 0), g_WorldMatrix)).xyz * In[0].fSize * 0.5f;
            vUp = normalize(mul(float4(vUp, 0), g_WorldMatrix)).xyz * In[0].fSize * 0.5f;
        }
        
        Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp + vLook, 1.f), matVP);
        Out[0].vTexcoord = float2(0.f, 0.f);
        Out[0].vLifeTime = In[0].vLifeTime;
        Out[0].vProjPos = Out[0].vPosition;
    
        Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp + vLook, 1.f), matVP);
        Out[1].vTexcoord = float2(1.f, 0.f);
        Out[1].vLifeTime = In[0].vLifeTime;
        Out[1].vProjPos = Out[1].vPosition;
        
        Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp + vLook, 1.f), matVP);
        Out[2].vTexcoord = float2(1.f, 1.f);
        Out[2].vLifeTime = In[0].vLifeTime;
        Out[2].vProjPos = Out[2].vPosition;
    
        Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp + vLook, 1.f), matVP);
        Out[3].vTexcoord = float2(0.f, 1.f);
        Out[3].vLifeTime = In[0].vLifeTime;
        Out[3].vProjPos = Out[3].vPosition;
        
        Out[0].vNormal = float4(0, 0, 0, 0);
        Out[1].vNormal = float4(0, 0, 0, 0);
        Out[2].vNormal = float4(0, 0, 0, 0);
        Out[3].vNormal = float4(0, 0, 0, 0);
    
        vector vSourDir, vDestDir, vNormal;
        vSourDir = float4(In[0].vPosition.xyz - vRight * 3 + vUp, 1.f) - float4(In[0].vPosition.xyz + vRight * 3 + vUp, 1.f);
        vDestDir = float4(In[0].vPosition.xyz - vRight * 3 - vUp, 1.f) - float4(In[0].vPosition.xyz - vRight * 3 + vUp, 1.f);
        vNormal = normalize(float4(cross(vSourDir.xyz, vDestDir.xyz), 0));
    
        Out[0].vNormal += vNormal;
        Out[1].vNormal += vNormal;
        Out[2].vNormal += vNormal;
    
        vSourDir = float4(In[0].vPosition.xyz - vRight * 3 - vUp, 1.f) - float4(In[0].vPosition.xyz + vRight * 3 + vUp, 1.f);
        vDestDir = float4(In[0].vPosition.xyz + vRight * 3 - vUp, 1.f) - float4(In[0].vPosition.xyz - vRight * 3 - vUp, 1.f);
        vNormal = normalize(float4(cross(vSourDir.xyz, vDestDir.xyz), 0));
    
        Out[0].vNormal += vNormal;
        Out[2].vNormal += vNormal;
        Out[3].vNormal += vNormal;
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
    if (g_bisBillboard)
    {
        float3 vRight = -normalize(g_CamMatrix._11_12_13) * In[0].fSize * 0.5f;
        float3 vUp = normalize(g_CamMatrix._21_22_23) * In[0].fSize * 0.5f;
        
        matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
        
        Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
        Out[0].vTexcoord = float2(0.f, 0.f);
        Out[0].vLifeTime = In[0].vLifeTime;
        
        Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
        Out[1].vTexcoord = float2(1.f, 0.f);
        Out[1].vLifeTime = In[0].vLifeTime;
     
        Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
        Out[2].vTexcoord = float2(1.f, 1.f);
        Out[2].vLifeTime = In[0].vLifeTime;
     
        Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
        Out[3].vTexcoord = float2(0.f, 1.f);
        Out[3].vLifeTime = In[0].vLifeTime;
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
        float3 vLook = normalize(In[0].TransformMatrix._31_32_33);
        float3 vRight = normalize(In[0].TransformMatrix._11_12_13) * In[0].fSize * 0.5f;
        float3 vUp = normalize(cross(vLook, vRight)) * In[0].fSize * 0.5f;
        matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
        
        
        if (!g_bisSpectrum)
        {
            vRight = normalize(mul(float4(vRight, 0), g_WorldMatrix)).xyz * In[0].fSize * 0.5f;
            vUp = normalize(mul(float4(vUp, 0), g_WorldMatrix)).xyz * In[0].fSize * 0.5f;
        }
        Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp + vLook, 1.f), matVP);
        Out[0].vTexcoord = float2(0.f, 0.f);
        Out[0].vLifeTime = In[0].vLifeTime;
    
        Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp + vLook, 1.f), matVP);
        Out[1].vTexcoord = float2(1.f, 0.f);
        Out[1].vLifeTime = In[0].vLifeTime;
        
        Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp + vLook, 1.f), matVP);
        Out[2].vTexcoord = float2(1.f, 1.f);
        Out[2].vLifeTime = In[0].vLifeTime;
    
        Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp + vLook, 1.f), matVP);
        Out[3].vTexcoord = float2(0.f, 1.f);
        Out[3].vLifeTime = In[0].vLifeTime;
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
    if (g_bisBillboard)
    {
        float3 vRight = -normalize(g_CamMatrix._11_12_13) * In[0].fSize * 0.5f;
        float3 vUp = normalize(g_CamMatrix._21_22_23) * In[0].fSize * 0.5f;
        
        matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
        
        Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
        Out[0].vTexcoord = float2(0.f, 0.f);
        Out[0].vLifeTime = In[0].vLifeTime;
        Out[0].vProjPos = Out[0].vPosition;
        
        Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
        Out[1].vTexcoord = float2(1.f, 0.f);
        Out[1].vLifeTime = In[0].vLifeTime;
        Out[1].vProjPos = Out[1].vPosition;
     
        Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
        Out[2].vTexcoord = float2(1.f, 1.f);
        Out[2].vLifeTime = In[0].vLifeTime;
        Out[2].vProjPos = Out[2].vPosition;
     
        Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
        Out[3].vTexcoord = float2(0.f, 1.f);
        Out[3].vLifeTime = In[0].vLifeTime;
        Out[3].vProjPos = Out[3].vPosition;
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
        float3 vLook = normalize(In[0].TransformMatrix._31_32_33);
        float3 vRight = normalize(In[0].TransformMatrix._11_12_13) * In[0].fSize * 0.5f;
        float3 vUp = normalize(cross(vLook, vRight)) * In[0].fSize * 0.5f;
        matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
        
        
        if (!g_bisSpectrum)
        {
            vRight = normalize(mul(float4(vRight, 0), g_WorldMatrix)).xyz * In[0].fSize * 0.5f;
            vUp = normalize(mul(float4(vUp, 0), g_WorldMatrix)).xyz * In[0].fSize * 0.5f;
        }
        
        Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
        Out[0].vTexcoord = float2(0.f, 0.f);
        Out[0].vLifeTime = In[0].vLifeTime;
        Out[0].vProjPos = Out[0].vPosition;
    
        Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
        Out[1].vTexcoord = float2(1.f, 0.f);
        Out[1].vLifeTime = In[0].vLifeTime;
        Out[1].vProjPos = Out[1].vPosition;
    
        Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
        Out[2].vTexcoord = float2(1.f, 1.f);
        Out[2].vLifeTime = In[0].vLifeTime;
        Out[2].vProjPos = Out[2].vPosition;
    
        Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
        Out[3].vTexcoord = float2(0.f, 1.f);
        Out[3].vLifeTime = In[0].vLifeTime;
        Out[3].vProjPos = Out[3].vPosition;
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
};


struct PS_NONLIGHT_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
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
    float4 vRimLight : SV_TARGET3;
};

struct PS_NONLIGHT_OUT
{
    float4 vDiffuse : SV_TARGET0;
};

struct PS_WEIGHT_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vWeight : SV_TARGET1;
};


/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NORMAL_OUT PS_NORMAL(PS_NORMAL_IN In)
{
    PS_NORMAL_OUT Out;
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + In.vLifeTime.x * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + In.vLifeTime.x * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + In.vLifeTime.x * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + In.vLifeTime.x * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    Out.vDiffuse = g_vColor * g_DiffuseTexture.Sample(MirrorSampler, DiffuseTexcoord);
    Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(NoneSampler, MaskTexcoord).r * saturate(In.vLifeTime.y - In.vLifeTime.x);
    if (Out.vDiffuse.a <= g_DissolveTexture.Sample(MirrorSampler, DissolveTexcoord).r || 0 > In.vLifeTime.x)
        discard;
    float3 vNormal = normalize(mul(vector(In.vNormal.xyz, 0), g_WorldMatrix)).xyz;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);
    Out.vRimLight = (0, 0, 0, 0);
    return Out;
}


/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_NONLIGHT(PS_NONLIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + In.vLifeTime.x * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + In.vLifeTime.x * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + In.vLifeTime.x * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + In.vLifeTime.x * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    Out.vDiffuse = g_vColor * g_DiffuseTexture.Sample(MirrorSampler, DiffuseTexcoord);
    Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(NoneSampler, MaskTexcoord).r * saturate(In.vLifeTime.y - In.vLifeTime.x);
    if (Out.vDiffuse.a < g_DissolveTexture.Sample(MirrorSampler, DissolveTexcoord).r || 0 > In.vLifeTime.x)
        discard;
    return Out;
}
/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_WEIGHT_OUT PS_WEIGHT(PS_WEIGHT_IN In)
{
    PS_WEIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + In.vLifeTime.x * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + In.vLifeTime.x * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + In.vLifeTime.x * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + In.vLifeTime.x * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    Out.vDiffuse = g_vColor * g_DiffuseTexture.Sample(MirrorSampler, DiffuseTexcoord);
    Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(NoneSampler, MaskTexcoord).r * saturate(In.vLifeTime.y - In.vLifeTime.x);
    if (Out.vDiffuse.a < g_DissolveTexture.Sample(MirrorSampler, DissolveTexcoord).r || 0 > In.vLifeTime.x)
        discard;
    if (0 >= Out.vDiffuse.a)
        discard;
    
    
    float linearDepth = 0.1 * 500 / (500.f - (In.vProjPos.z / In.vProjPos.w) * (500 - 0.1));
    float weight = saturate(pow(1 - linearDepth / 500, 3));
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight;
    Out.vWeight.r = Out.vDiffuse.a * weight;
    Out.vWeight.g = Out.vDiffuse.a;
    Out.vWeight.b = weight;
    Out.vDiffuse.a = 1;
    Out.vWeight.a = 1;
    
    return Out;
}

PS_NONLIGHT_OUT PS_SMOK(PS_NONLIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + In.vLifeTime.x * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + In.vLifeTime.x * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + In.vLifeTime.x * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + In.vLifeTime.x * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    Out.vDiffuse = g_vColor * g_DiffuseTexture.Sample(MirrorSampler, DiffuseTexcoord);
    Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(NoneSampler, MaskTexcoord).r * saturate(In.vLifeTime.y - In.vLifeTime.x);
    if (Out.vDiffuse.a < g_DissolveTexture.Sample(MirrorSampler, DissolveTexcoord).r || 0 > In.vLifeTime.x)
        discard;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_WEIGHT_OUT PS_SPHERE(PS_WEIGHT_IN In)
{
    PS_WEIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((In.vTexcoord.x + g_fMaskUV.x + In.vLifeTime.x * g_fMaskUVSpeed.x) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + In.vLifeTime.x * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + In.vLifeTime.x * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + In.vLifeTime.x * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + In.vLifeTime.x * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + In.vLifeTime.x * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    float diffuse = (g_DiffuseTexture.Sample(MirrorSampler, DiffuseTexcoord).r - 0.5) * 2 * saturate((In.vLifeTime.y - In.vLifeTime.x));
    Out.vDiffuse = g_vColor;
    //Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x + diffuse * 0.05, MaskTexcoord.y + diffuse * 0.05)).r * saturate(In.vLifeTime.y - In.vLifeTime.x);
    float2 texcoord = float2(MaskTexcoord.x * 1.2 + diffuse * 0.2 - 0.2 * 0.5, MaskTexcoord.y * 1.2 + diffuse * 0.2 - 0.2 * 0.5);
    Out.vDiffuse.a *= length(float2(0.5, 0.5) - texcoord) * 2;
    Out.vDiffuse.a = pow(Out.vDiffuse.a, 10);
    //Out.vDiffuse.a *= 0.3;
    Out.vDiffuse.a *= saturate((In.vLifeTime.y - In.vLifeTime.x) * 1.5);
    if (0 >= Out.vDiffuse.a || 0.5 < length(float2(0.5, 0.5) - texcoord))
        discard;
    
    
    float linearDepth = 0.1 * 500 / (500.f - (In.vProjPos.z / In.vProjPos.w) * (500 - 0.1));
    float weight = saturate(pow(1 - linearDepth / 500, 3));
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight;
    Out.vWeight.r = Out.vDiffuse.a * weight;
    Out.vWeight.g = Out.vDiffuse.a;
    Out.vWeight.b = weight;
    Out.vDiffuse.a = 1;
    Out.vWeight.a = 1;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_WEIGHT_OUT PS_POWER(PS_WEIGHT_IN In)
{
    PS_WEIGHT_OUT Out;
    
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
    
    float4 col = g_DissolveTexture.Sample(DefaultSampler, uvDistorted);
    
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
    //Out.vDiffuse.a *= col.r - (smoothstep(min((In.vLifeTime.x / In.vLifeTime.y) * 0.9, 0.5), min((In.vLifeTime.x / In.vLifeTime.y) * 0.9, 0.5) - 0.1, dist));
    float linearDepth = 0.1 * 500 / (500.f - (In.vProjPos.z / In.vProjPos.w) * (500 - 0.1));
    float weight = saturate(pow(1 - linearDepth / 500, 3));
    if (0 >= Out.vDiffuse.a)
        discard;
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight;
    Out.vWeight.r = Out.vDiffuse.a * weight;
    Out.vWeight.g = Out.vDiffuse.a;
    Out.vWeight.b = weight;
    Out.vDiffuse.a = 1;
    Out.vWeight.a = 1;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_DASH_DISTORTION(PS_NONLIGHT_IN In)
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
    
    float ring = smoothstep(saturate((In.vLifeTime.x) / In.vLifeTime.y), saturate((In.vLifeTime.x) / In.vLifeTime.y) - 0.01, dist);
    
    float3 noise = g_DiffuseTexture.Sample(DefaultSampler, tex).rgb;
    
    float2 distort = (noise.rb * 2.0 - 1.0) * 0.2;
    
    float2 uvDistorted = uv + distort * ring;
    
    float4 col = g_DissolveTexture.Sample(DefaultSampler, uvDistorted);
    
    float mask = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
    col.rgb *= ring * mask;
    //col.a *= g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r;
    Out.vDiffuse = g_vColor * col;
    
    
    float diffuse = (g_DiffuseTexture.Sample(MirrorSampler, tex).r - 0.5) * 2 * saturate((In.vLifeTime.y - In.vLifeTime.x)) * 3;
    
   //Out.vDiffuse.a *= length(float2(0.5, 0.5) - In.vTexcoord) * 2;
   //Out.vDiffuse.a = pow(Out.vDiffuse.a, 3);
   //Out.vDiffuse.a *= saturate((In.vLifeTime.y - In.vLifeTime.x) * 1.5);
    Out.vDiffuse.a *= g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r * saturate((In.vLifeTime.y - In.vLifeTime.x) * 3);
    Out.vDiffuse.rgb *= saturate(Out.vDiffuse.a);
    if (0 >= Out.vDiffuse.a || 0.5 < length(float2(0.5, 0.5) - In.vTexcoord))
       discard;
    //Out.vDiffuse.a *= col.r;
    //if (0 >= Out.vDiffuse.a)
    //    discard;
    return Out;
}


/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_WEIGHT_OUT PS_SHOCK(PS_WEIGHT_IN In)
{
    PS_WEIGHT_OUT Out;
    
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
    
    float4 col = g_DissolveTexture.Sample(DefaultSampler, uvDistorted);
    
    float mask = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
    col.rgb *= ring * mask;
    //col.a *= g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r;
    Out.vDiffuse = g_vColor;
    
    //Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x + diffuse * 0.05, MaskTexcoord.y + diffuse * 0.05)).r * saturate(In.vLifeTime.y - In.vLifeTime.x);
    //float2 texcoord = float2(MaskTexcoord.x * 1.2 + diffuse * 0.05 - 0.2 * 0.5, MaskTexcoord.y * 1.2 + diffuse * 0.05 - 0.2 * 0.5);
    
    
    //Out.vDiffuse.a *= length(float2(0.5, 0.5) - texcoord) * 2;
    //Out.vDiffuse.a = pow(Out.vDiffuse.a, 10);
    //Out.vDiffuse.a *= saturate((In.vLifeTime.y - In.vLifeTime.x) * 1.5);
    //if (0 >= Out.vDiffuse.a || 0.5 < length(float2(0.5, 0.5) - texcoord))
    //    discard;
    Out.vDiffuse.a *= col.r * saturate(In.vLifeTime.y - In.vLifeTime.x);
    //Out.vDiffuse.a *= col.r - (smoothstep(min((In.vLifeTime.x / In.vLifeTime.y) * 0.9, 0.5), min((In.vLifeTime.x / In.vLifeTime.y) * 0.9, 0.5) - 0.1, dist));
    float linearDepth = 0.1 * 500 / (500.f - (In.vProjPos.z / In.vProjPos.w) * (500 - 0.1));
    float weight = saturate(pow(1 - linearDepth / 500, 3));
    if (0 >= Out.vDiffuse.a)
        discard;
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight;
    Out.vWeight.r = Out.vDiffuse.a * weight;
    Out.vWeight.g = Out.vDiffuse.a;
    Out.vWeight.b = weight;
    Out.vDiffuse.a = 1;
    Out.vWeight.a = 1;
    return Out;
}

technique11 DefaultTechnique
{
    pass NonBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NORMAL_BILLBOARD();
        PixelShader = compile ps_5_0 PS_NORMAL();
    }

    pass NonLight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_NONLIGHT();
    }

    pass Weight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_WEIGHT();
    }

    pass Smok
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_SMOK();
    }

    pass SPHERE
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_SPHERE();
    }

    pass Power
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_POWER();
    }

    pass Dash_Distortion
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_DASH_DISTORTION();
    }

    pass Shock
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_SHOCK();
    }
}