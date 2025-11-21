#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_MaskTexture, g_DiffuseTexture, g_NormalTexture;
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
vector g_vCamPosition;
int g_iSizeCount;
bool g_bisBillboard;
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
    Out.vOnePos = mul(vector(In.vPosition, 1), g_WorldMatrix);
    Out.vSeed = id;
    Out.TransformMatrix = mul(In.TransformMatrix, g_WorldMatrix);
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
    
    float3 vLook = (g_bisBillboard ? g_vCamPosition - In[0].vPosition : In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * g_fSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * g_fSize.y * 0.5f;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    //vRight.xyz = vRight.xyz;
    //vUp.xyz = vUp.yzx;
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


[maxvertexcount(6)]
void GS_NONLIGHT_BILLBOARD(point GS_IN In[1], inout TriangleStream<GS_NONLIGHT_OUT> OutStream)
{
    GS_NONLIGHT_OUT Out[4];
    
    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * g_fSize.x * In[0].fSize * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * g_fSize.y * In[0].fSize * 0.5f;
        
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

[maxvertexcount(6)]
void GS_WEIGHT_BILLBOARD(point GS_IN In[1], inout TriangleStream<GS_WEIGHT_OUT> OutStream)
{
    GS_WEIGHT_OUT Out[4];
    if (g_bisBillboard)
    {
        float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
        float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * g_fSize.x * In[0].fSize * 0.5f;
        float3 vUp = normalize(cross(vLook, vRight)) * g_fSize.y * In[0].fSize * 0.5f;
        
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
        float3 vRight = normalize(In[0].TransformMatrix._11_12_13);
        float3 vUp = normalize(cross(vLook, vRight));
        matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
        
        float angle = radians(g_fAngle);
    
        float s = sin(angle);
        float c = cos(angle);
    
        float3 vRightRot = vRight * c + vLook * s;
        float3 vUpRot = vLook * c - vRight * s;
    
        float3 vR = vRightRot * (g_fSize.x * In[0].fSize * 0.5f);
        float3 vU = vUpRot * (g_fSize.y * In[0].fSize * 0.5f);
        vLook *= g_fSize.x * In[0].fSize;
        Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vR + vU, 1.f), matVP);
        Out[0].vTexcoord = float2(0.f, 0.f);
        Out[0].vLifeTime = In[0].vLifeTime;
        Out[0].vProjPos = Out[0].vPosition;
    
        Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vR + vU, 1.f), matVP);
        Out[1].vTexcoord = float2(1.f, 0.f);
        Out[1].vLifeTime = In[0].vLifeTime;
        Out[1].vProjPos = Out[1].vPosition;
    
        Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vR - vU, 1.f), matVP);
        Out[2].vTexcoord = float2(1.f, 1.f);
        Out[2].vLifeTime = In[0].vLifeTime;
        Out[2].vProjPos = Out[2].vPosition;
    
        Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vR - vU, 1.f), matVP);
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
    
    float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
    int iU = (In.vLifeTime.x / fFPS);
    int iV = In.vLifeTime.x / fFPS / g_iUV.x;
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, fTexcoord);
    if (Out.vDiffuse.a <= 0.5f)
        discard;
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, fTexcoord);
    float3 vNormal = mul(normalize(vNormalDesc.xyz), (float3x3) g_WorldMatrix);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);
    return Out;
}


/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_MASK(PS_NONLIGHT_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
    int iU = (In.vLifeTime.x / fFPS);
    int iV = In.vLifeTime.x / fFPS / g_iUV.x;
    
    

    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a) * saturate(In.vLifeTime.y - In.vLifeTime.x);
    
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_WEIGHT_OUT PS_WEIGHT(PS_WEIGHT_IN In)
{
    PS_WEIGHT_OUT Out;
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 >= In.vLifeTime.x)
        discard;
    float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
    int iU = (In.vLifeTime.x / fFPS);
    int iV = In.vLifeTime.x / fFPS / g_iUV.x;
    
    

    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a) * saturate(In.vLifeTime.y - In.vLifeTime.x);
    
    float depth = In.vProjPos.w / 500;
    float weight = saturate(pow(1 - depth, 3));
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight;
    Out.vWeight.r = Out.vDiffuse.a;
    Out.vWeight.g = weight;
    Out.vDiffuse.a = 1;
    Out.vWeight.a = 1;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_WEIGHT_OUT PS_ELECTRIC(PS_WEIGHT_IN In)
{
    PS_WEIGHT_OUT Out;
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 > In.vLifeTime.x)
        discard;
    float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
    int i = In.vSeed % (g_iUV.x * g_iUV.y);
    int iU = i % g_iUV.x;
    int iV = i / g_iUV.x;
    
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a) * (1 - abs(1 - (In.vLifeTime.x / In.vLifeTime.y) * 2) - abs(1 - ((In.vTexcoord.y) + (In.vLifeTime.x / In.vLifeTime.y))));
    
    float depth = In.vProjPos.w / 500;
    float weight = saturate(pow(1 - depth, 3));
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight;
    Out.vWeight.r = Out.vDiffuse.a;
    Out.vWeight.g = weight;
    Out.vDiffuse.a = 1;
    Out.vWeight.a = 1;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_WEIGHT_OUT PS_ELECTRIC2(PS_WEIGHT_IN In)
{
    PS_WEIGHT_OUT Out;
    
    if (In.vLifeTime.y < In.vLifeTime.x || 0 > In.vLifeTime.x)
        discard;
    float fFPS = In.vLifeTime.y / (g_iUV.x * g_iUV.y);
    int i = In.vSeed % (g_iUV.x * g_iUV.y);
    int iU = i % g_iUV.x;
    int iV = i / g_iUV.x;
    
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, -In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a) * (1 - abs(1 - (In.vLifeTime.x / In.vLifeTime.y) * 2) - abs(1 - ((In.vTexcoord.y) + (In.vLifeTime.x / In.vLifeTime.y))));
    
    float depth = In.vProjPos.w / 500;
    float weight = saturate(pow(1 - depth, 3));
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight;
    Out.vWeight.r = Out.vDiffuse.a;
    Out.vWeight.g = weight;
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
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONLIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_MASK();
    }

    pass Weight
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_WEIGHT();
    }

    pass Electric
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_ELECTRIC();
    }

    pass Electric2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_WEIGHT_BILLBOARD();
        PixelShader = compile ps_5_0 PS_ELECTRIC2();
    }
}