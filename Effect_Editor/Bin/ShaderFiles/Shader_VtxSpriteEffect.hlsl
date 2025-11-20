#include "Engine_Shader_Defines.hlsli"


matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_MaskTexture, g_DiffuseTexture, g_NormalTexture;
float2 g_fSize;
int2 g_iUV;
float g_fFPS, g_fTime;
vector g_vColor = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vLifeTime : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vNormal : TEXCOORD2;
};



[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];
    
    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * g_fSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * g_fSize.y * 0.5f;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vProjPos = Out[0].vPosition;
    
    Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vProjPos = Out[1].vPosition;
    
    Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vProjPos = Out[2].vPosition;
    
    Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
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

/* 출력된 정점 위치벡터의 w값으로 모든 성분을 나눈다 -> 투영스페이스로 변환 */ 
/* 정점의 위치에 대해서 뷰포트 변환을 수행한다 */ 
/* 정점의 모든 정보를 보간하여 픽셀을 만든다. -> 래스터라이즈 */ 

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vNormal : TEXCOORD2;
};

struct PS_NORMAL_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vWeight : SV_TARGET1;
};

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NORMAL_OUT PS_MAIN(PS_IN In)
{
    PS_NORMAL_OUT Out;
    int iU = (g_fTime / g_fFPS);
    int iV = g_fTime / g_fFPS / g_iUV.x;
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
PS_OUT PS_MASK(PS_IN In)
{
    PS_OUT Out;
    int iU = (g_fTime / g_fFPS);
    int iV = g_fTime / g_fFPS / g_iUV.x;
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a);
    
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
PS_OUT PS_NONLIGHT(PS_IN In)
{
    PS_OUT Out;
    int iU = (g_fTime / g_fFPS);
    int iV = g_fTime / g_fFPS / g_iUV.x;
    float2 fTexcoord = float2(In.vTexcoord.x / g_iUV.x + 1.0 / g_iUV.x * iU, In.vTexcoord.y / g_iUV.y + 1.0 / g_iUV.y * iV);
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= min(g_MaskTexture.Sample(DefaultSampler, fTexcoord).r, g_MaskTexture.Sample(DefaultSampler, fTexcoord).a);
    return Out;
}

technique11 DefaultTechnique
{
    pass SpriteEffect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass SpriteMaskEffect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MASK();
    }
    pass SpriteNonLightEffect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_NONLIGHT();
    }
}