#include "Client_Shader_Utils.hlsli"
#include "Client_Shader_VtxMesh_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_EmissiveTexture;
Texture2D g_ORMTexture;

//림라이트용 변수
vector g_vCamPosition;
float g_fRimLightPower;
float g_fRimLightStrength;
float4 g_vRimLightColor;
float g_fFar;
/* 정점 쉐이더 : */
/* 정점에 대한 셰이딩 == 정점에 필요한 연산을 수행한다 == 정점의 상태변환(월드, 뷰, 투영) + 추가변환 */
/* 정점의 구성 정보를 수정, 변경한다 */ 

// for cascade 
matrix g_LightViewMatrix[5];
matrix g_LightProjMatrix[5];

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(mul(vector(In.vBinormal, 0.f), g_WorldMatrix)).xyz;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    return Out;
}

VS_OUT_STATIC_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_STATIC_SHADOW Out;

    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vProjPos = Out.vPosition;

    return Out;
}

[maxvertexcount(CASCADE_LEVEL * 3)]
void GS_MAIN_SHADOW(triangle VS_OUT_SHADOW InTri[3], inout TriangleStream<GS_OUT_SHADOW> OutStream)
{
    //CASCADE LEVEL 순회하면서 한번에 찍게 하기
    [unroll]
    for (uint iCount = 0; iCount < CASCADE_LEVEL; ++iCount)
    {
        matrix matLightVP = mul(g_LightViewMatrix[iCount], g_LightProjMatrix[iCount]);

        GS_OUT_SHADOW Out;

        // 정점 3개당 삼각형 하나로 
        // 취급해서 세팅해주기
        [unroll]
        for (int iTri = 0; iTri < 3; ++iTri)
        {
            float4 vClip = mul(InTri[iTri].vPosition, matLightVP);

            Out.vPosition = vClip;
            Out.vProjPos = vClip;
            Out.iSlice = iCount;

            OutStream.Append(Out);
        }

        OutStream.RestartStrip();
    }
}


/* 출력된 정점 위치벡터의 w값으로 모든 성분을 나눈다 -> 투영스페이스로 변환 */ 
/* 정점의 위치에 대해서 뷰포트 변환을 수행한다 */ 
/* 정점의 모든 정보를 보간하여 픽셀을 만든다. -> 래스터라이즈 */ 

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
    
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 1.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_SKYBOX(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(0.f, 0.f, 0.f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 1.0f);
    Out.vORM = float4(0.f, 0.f, 0.f, 0.f);
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MOON(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.r < 0.001f && vMtrlDiffuse.g < 0.001f && vMtrlDiffuse.b < 0.001f)
        discard;
    
    float3 vNormal = normalize(In.vNormal);
    float3 vLightDir = normalize(float3(0.f, 0.f, 1.f));
    
    float fDiffuse = max(0.f, dot(vNormal, -vLightDir));
    
    float3 vAmbientColor = float3(0.1f, 0.1f, 0.12f);
    float3 vLightColor = float3(0.8f, 0.8f, 0.9f);
    float3 vEmissiveColor = float3(0.3f, 0.45f, 0.7f);
    
    float3 finalDiffuseColor = vMtrlDiffuse.rgb * (vAmbientColor + fDiffuse * vLightColor);
    
    Out.vDiffuse.rgb = finalDiffuseColor + vEmissiveColor;
    Out.vDiffuse *= 2.f;
    
    Out.vDiffuse.a = vMtrlDiffuse.a;
    Out.vNormal = float4(0.5f, 0.5f, 0.5f, 0.5f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 1.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    
    return Out;
}

PS_OUT PS_MAIN_EMISSIVE(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
    
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 1.0f);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord) * 1.5f;
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    
    return Out;
}

PS_OUT_STATIC_SHADOW PS_MAIN_SHADOW(PS_IN_STATIC_SHADOW In)
{
    PS_OUT_STATIC_SHADOW Out = (PS_OUT_STATIC_SHADOW) 0;
    
    Out.vShadowLightDepth.x = In.vProjPos.z / In.vProjPos.w;
    
    return Out;
}

VS_OUT_SHADOW VS_MAIN_CASCADE_SHADOW(VS_IN In)
{
    VS_OUT_STATIC_SHADOW Out;

    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;

    return Out;
}

PS_OUT_SHADOW PS_MAIN_CASCADE_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    
    Out.vShadowLightDepth.x = In.vProjPos.z / In.vProjPos.w;
    
    return Out;
}

technique11 DefaultTechnique
{ 
    // idx 0
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    // idx 1
    pass Shadow
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }
    // idx 2
    pass SKYBOX 
    {
        SetRasterizerState(RS_Cull_None); 
        SetDepthStencilState(DSS_DepthTest_ON_Write_OFF, 0); 
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN(); 
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SKYBOX();
    }
    // idx 3 
    pass Blend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    // idx 4 
    pass Emissive
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE();
    }
    // idx 5
    pass MOON
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthTest_ON_Write_OFF, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MOON();
    }
    // idx 6
    pass CascadeShadow
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_CASCADE_SHADOW();
        GeometryShader = compile gs_5_0 GS_MAIN_SHADOW();
        PixelShader = compile ps_5_0 PS_MAIN_CASCADE_SHADOW();
    }

    // idx 7
    pass Tree
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}