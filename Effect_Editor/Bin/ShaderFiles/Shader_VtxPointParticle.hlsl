#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_MaskTexture, g_DiffuseTexture, g_DissolveTexture;
vector g_vColor = vector(1.f, 1.f, 1.f, 1.f);
vector g_vSize = vector(1.f, 0.f, 1.f, 0.f);
vector g_vCamPosition;
int g_iSizeCount;
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
    float  fSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
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

    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float fSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vNormal : TEXCOORD3;
};

[maxvertexcount(6)]
void GS_BILLBOARD(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];
    
    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].fSize * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].fSize * 0.5f;
    
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

[maxvertexcount(6)]
void GS_RECT(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];
    
    float3 vLook = In[0].vPosition.xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].fSize * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].fSize * 0.5f;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight * 6 + vUp, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vProjPos = Out[0].vPosition;
    
    
    Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight * 6 + vUp, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vProjPos = Out[1].vPosition;
    
    Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight * 6 - vUp, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vProjPos = Out[2].vPosition;
    Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight * 6 - vUp, 1.f), matVP);
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

//[maxvertexcount(6)]
//void GS_VANILLA(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
//{
//    GS_OUT Out[4];
//    
//    float3 vLook = (In[0].vPosition).xyz;
//    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].fSize * 0.5f;
//    float3 vUp = normalize(cross(vLook, vRight)) * In[0].fSize * 0.5f;
//    
//    
//    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
//    
//    Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
//    Out[0].vTexcoord = float2(0.f, 0.f);
//    Out[0].vLifeTime = In[0].vLifeTime;
//    Out[0].vProjPos = Out[0].vPosition;
//    
//    Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
//    Out[1].vTexcoord = float2(1.f, 0.f);
//    Out[1].vLifeTime = In[0].vLifeTime;
//    Out[1].vProjPos = Out[1].vPosition;
//    
//    Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
//    Out[2].vTexcoord = float2(1.f, 1.f);
//    Out[2].vLifeTime = In[0].vLifeTime;
//    Out[2].vProjPos = Out[2].vPosition;
//    
//    Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
//    Out[3].vTexcoord = float2(0.f, 1.f);
//    Out[3].vLifeTime = In[0].vLifeTime;
//    Out[3].vProjPos = Out[3].vPosition;
//    
//    
//    OutStream.Append(Out[0]);
//    OutStream.Append(Out[1]);
//    OutStream.Append(Out[2]);
//    OutStream.RestartStrip();
//    
//    OutStream.Append(Out[0]);
//    OutStream.Append(Out[2]);
//    OutStream.Append(Out[3]);
//    OutStream.RestartStrip();
//}

/* 출력된 정점 위치벡터의 w값으로 모든 성분을 나눈다 -> 투영스페이스로 변환 */ 
/* 정점의 위치에 대해서 뷰포트 변환을 수행한다 */ 
/* 정점의 모든 정보를 보간하여 픽셀을 만든다. -> 래스터라이즈 */ 

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vNormal : TEXCOORD3;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

struct PS_LIGHT_OUT
{
    float4 vDiffuse :   SV_TARGET0;
    float4 vNormal :    SV_TARGET1;
    float4 vDepth :     SV_TARGET2;
    float4 vRimLight :  SV_TARGET3;
};



/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_LIGHT_OUT PS_MAIN(PS_IN In)
{
    PS_LIGHT_OUT Out;
    float4 fireFront = g_DissolveTexture.Sample(DefaultSampler, float2(In.vTexcoord.x + In.vLifeTime.x, In.vTexcoord.y + In.vLifeTime.x));
    float4 fireBack = g_DissolveTexture.Sample(DefaultSampler, float2(In.vTexcoord.x - In.vLifeTime.x, In.vTexcoord.y + In.vLifeTime.x));
    //Out.vColor = g_vColor * (g_DiffuseTexture.Sample(MirrorSampler, float2(In.vPosition.x / 1000, In.vPosition.y / 1000)));
    //Out.vColor = g_vColor * (g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord));
    Out.vDiffuse = g_vColor * (g_DiffuseTexture.Sample(DefaultSampler, float2(In.vTexcoord.x + In.vLifeTime.x, In.vTexcoord.y + In.vLifeTime.x)) + g_DiffuseTexture.Sample(DefaultSampler, float2(In.vTexcoord.x - In.vLifeTime.x, In.vTexcoord.y + In.vLifeTime.x)));
    Out.vDiffuse.a = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r;
    Out.vDiffuse.a *= ((fireFront.r + fireBack.r) / 2) * ((In.vLifeTime.y - In.vLifeTime.x) / In.vLifeTime.y);
    //Out.vDiffuse = g_vColor;
    if (Out.vDiffuse.a <= 0.1f || 0 > In.vLifeTime.x)
        discard;
    Out.vDiffuse.a = g_vColor.a;
    //Out.vDiffuse *= g_vColor;
    
    //float weight = saturate(pow(1 - (In.vProjPos.z / In.vProjPos.w / 500), 3));
    //
    //Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight;
    //Out.vNormal.r = Out.vDiffuse.a;
    //Out.vNormal.g = Out.vDiffuse.a * weight;
    //Out.vDiffuse.a = 1;
    //Out.vNormal.a = 1;
    
    
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_LIGHT_OUT PS_RECT(PS_IN In)
{
    PS_LIGHT_OUT Out;
    float4 fireFront = g_DissolveTexture.Sample(DefaultSampler, float2(In.vTexcoord.x + In.vLifeTime.x, In.vTexcoord.y + In.vLifeTime.x));
    float4 fireBack = g_DissolveTexture.Sample(DefaultSampler, float2(In.vTexcoord.x - In.vLifeTime.x, In.vTexcoord.y + In.vLifeTime.x));
    //Out.vColor = g_vColor * (g_DiffuseTexture.Sample(MirrorSampler, float2(In.vPosition.x / 1000, In.vPosition.y / 1000)));
    //Out.vColor = g_vColor * (g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord));
    Out.vDiffuse = g_vColor * (g_DiffuseTexture.Sample(DefaultSampler, float2(In.vTexcoord.x + In.vLifeTime.x, In.vTexcoord.y + In.vLifeTime.x)) + g_DiffuseTexture.Sample(DefaultSampler, float2(In.vTexcoord.x - In.vLifeTime.x, In.vTexcoord.y + In.vLifeTime.x)));
    Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r;
    Out.vDiffuse.a *= ((fireFront.r + fireBack.r) / 2) * ((In.vLifeTime.y - In.vLifeTime.x) / In.vLifeTime.y);
    
    if (Out.vDiffuse.a <= 0.1f || 0 > In.vLifeTime.x)
        discard;
    float3 vNormal = normalize(mul(vector(In.vNormal.xyz, 0), g_WorldMatrix)).xyz;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    //Out.vNormal = In.vNormal;

    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);
    //Out.vRimLight = (0, 0, 0, 0);
    //if (Out.vDiffuse.a <= 0.1f || 0 > In.vLifeTime.x)
    //    discard;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_LIGHT_OUT PS_SLASH(PS_IN In)
{
    PS_LIGHT_OUT Out;
    Out.vDiffuse = g_vColor;
    
    Out.vDiffuse *= ((In.vTexcoord.x + (In.vLifeTime.x / In.vLifeTime.y) * 2 - 1) * 0.5) * ((1 - In.vTexcoord.y) * 5) * 5;
    Out.vDiffuse *= g_DissolveTexture.Sample(DefaultSampler, float2(In.vTexcoord.x / 3, In.vTexcoord.y)) * (1 - (In.vTexcoord.x + (In.vLifeTime.x / In.vLifeTime.y) * 2 - 1)) +
    g_vColor * ((In.vTexcoord.x + (In.vLifeTime.x / In.vLifeTime.y) * 2 - 1));
    
    //Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(DefaultSampler, In.vTexcoord).r;
    //Out.vDiffuse.a *= ((fireFront.r + fireBack.r) / 2) * ((In.vLifeTime.y - In.vLifeTime.x) / In.vLifeTime.y);
    
    Out.vDiffuse.a = g_MaskTexture.Sample(NoneSampler, float2(In.vTexcoord.x + (In.vLifeTime.x / In.vLifeTime.y) * 2 - 1, In.vTexcoord.y)).r;
    
    if (Out.vDiffuse.a <= 0.1f || 0 > In.vLifeTime.x)
        discard;
    float3 vNormal = normalize(mul(vector(In.vNormal.xyz, 0), g_WorldMatrix)).xyz;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    //Out.vNormal = In.vNormal;

    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);
    //Out.vRimLight = (0, 0, 0, 0);
    //if (Out.vDiffuse.a <= 0.1f || 0 > In.vLifeTime.x)
    //    discard;
    return Out;
}

//PS_LIGHT_OUT PS_LIGHT(PS_IN In)
//{
//    PS_LIGHT_OUT Out;
//    Out.vDiffuse = g_vColor * (g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord));
//    
//    
//    vector vNormalDesc = float4(0.5f, 0.5f, 1.f, 1.f) * (g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord));
//    //float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
//    
//    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
//    float3 vNormal = mul(float4(vNormalDesc.xyz * 2.f - 1.f, 0), matVP);
//        
//    Out.vNormal = vNormal;
//    
//    //Out.vNormal = float4(0.5f, 0.5f, 1.f, 1.f);
//    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);
//    Out.vRimLight = (0, 0, 0, 0);
//    if (Out.vDiffuse.a <= 0.1f || 0 > In.vLifeTime.x)
//        discard;
//    return Out;
//}
//PS_OUT PS_PIXEL(PS_IN In)
//{
//    PS_OUT Out;
//    Out.vColor = g_vColor;
//    Out.vColor.a *= saturate(In.vLifeTime.y - In.vLifeTime.x);
//    return Out;
//}

technique11 DefaultTechnique
{ 
    pass Billboard
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        //SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_BILLBOARD();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Rect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_BILLBOARD();
        PixelShader = compile ps_5_0 PS_RECT();
    }

    pass Slash
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_RECT();
        PixelShader = compile ps_5_0 PS_SLASH();
    }

    //pass vanilla
    //{
    //    SetRasterizerState(RS_Default);
    //    SetDepthStencilState(DSS_Default, 0);
    //    SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    //    VertexShader = compile vs_5_0 VS_MAIN();
    //    GeometryShader = compile gs_5_0 GS_BILLBOARD();
    //    PixelShader = compile ps_5_0 PS_LIGHT();
    //}

    //pass Frame
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN();
    //    PixelShader = compile ps_5_0 PS_MAIN();
    //}
    //pass AlphaBlend
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN();
    //    PixelShader = compile ps_5_0 PS_MAIN();
    //}


 
    

 
}