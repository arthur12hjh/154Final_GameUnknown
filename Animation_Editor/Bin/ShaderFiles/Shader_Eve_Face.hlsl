#include "Client_Shader_Utils.hlsli"
#include "Client_Shader_VtxAnimMesh_Defines.hlsli"

/* 깊이 렌더타겟 마스킹용 */
bool g_IsMaskingDepthB;
bool g_IsMaskingDepthW;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_PreWorldMatrix, g_PreViewMatrix;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_OpacityTexture;
Texture2D g_EmissiveTexture;
Texture2D g_ORMTexture;
Texture2D g_ORSSTexture;
Texture2D g_SpecDetailTexture;
Texture2D g_SSSAOTexture;

// for cascade 
matrix g_LightViewMatrix[CASCADE_LEVEL];
matrix g_LightProjMatrix[CASCADE_LEVEL];

//림라이트용 변수
vector g_vCamPosition;
float g_fRimLightPower;
float g_fRimLightStrength;
float4 g_vRimLightColor;

/* 메시다 ㅇ영향을 주는 뼈들의 집합*/
matrix g_OffsetMatrices[512];

StructuredBuffer<BoneTransformMatrix> g_BoneMatrixBuffer : register(t16);
StructuredBuffer<BoneTransformMatrix> g_PreBoneMatrixBuffer : register(t17);


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
          
// CPU와 동일한 행렬 순서
    float4x4 MatrixX = mul(g_OffsetMatrices[In.vBlendIndex.x], g_BoneMatrixBuffer[In.vBlendIndex.x].BoneCombinedTransformMatrix);
    float4x4 MatrixY = mul(g_OffsetMatrices[In.vBlendIndex.y], g_BoneMatrixBuffer[In.vBlendIndex.y].BoneCombinedTransformMatrix);
    float4x4 MatrixZ = mul(g_OffsetMatrices[In.vBlendIndex.z], g_BoneMatrixBuffer[In.vBlendIndex.z].BoneCombinedTransformMatrix);
    float4x4 MatrixW = mul(g_OffsetMatrices[In.vBlendIndex.w], g_BoneMatrixBuffer[In.vBlendIndex.w].BoneCombinedTransformMatrix);
    
    matrix BoneMatrix = MatrixX * In.vBlendWeight.x +
        MatrixY * In.vBlendWeight.y +
        MatrixZ * In.vBlendWeight.z +
        MatrixW * In.vBlendWeight.w;
    
    /* 스키닝 */
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
    
    matrix matWV, matWVP, matOldWV, matOldWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    /* Out.vPosition.xy => 시야각에 있는 점들을 90에 맞춰준다 */ 
    /* Out.vPosition.z => n~f사이에 있는 점들의 z를 0 ~ f로 바꿔준다. */   
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(mul(vector(In.vBinormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;

    float4x4 MatrixX = mul(g_OffsetMatrices[In.vBlendIndex.x], g_BoneMatrixBuffer[In.vBlendIndex.x].BoneCombinedTransformMatrix);
    float4x4 MatrixY = mul(g_OffsetMatrices[In.vBlendIndex.y], g_BoneMatrixBuffer[In.vBlendIndex.y].BoneCombinedTransformMatrix);
    float4x4 MatrixZ = mul(g_OffsetMatrices[In.vBlendIndex.z], g_BoneMatrixBuffer[In.vBlendIndex.z].BoneCombinedTransformMatrix);
    float4x4 MatrixW = mul(g_OffsetMatrices[In.vBlendIndex.w], g_BoneMatrixBuffer[In.vBlendIndex.w].BoneCombinedTransformMatrix);

    matrix BoneMatrix =
        MatrixX * In.vBlendWeight.x +
        MatrixY * In.vBlendWeight.y +
        MatrixZ * In.vBlendWeight.z +
        MatrixW * In.vBlendWeight.w;

    float4 vSkinnedLocal = mul(float4(In.vPosition, 1.f), BoneMatrix);

    // 월드까지만
    Out.vPosition = mul(vSkinnedLocal, g_WorldMatrix);

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

VS_OUT_MOTIONBLUR VS_MAIN_MOTIONBLUR(VS_IN In)
{
    VS_OUT_MOTIONBLUR Out;
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    // CPU와 동일한 행렬 순서
    float4x4 MatrixX = mul(g_OffsetMatrices[In.vBlendIndex.x], g_BoneMatrixBuffer[In.vBlendIndex.x].BoneCombinedTransformMatrix);
    float4x4 MatrixY = mul(g_OffsetMatrices[In.vBlendIndex.y], g_BoneMatrixBuffer[In.vBlendIndex.y].BoneCombinedTransformMatrix);
    float4x4 MatrixZ = mul(g_OffsetMatrices[In.vBlendIndex.z], g_BoneMatrixBuffer[In.vBlendIndex.z].BoneCombinedTransformMatrix);
    float4x4 MatrixW = mul(g_OffsetMatrices[In.vBlendIndex.w], g_BoneMatrixBuffer[In.vBlendIndex.w].BoneCombinedTransformMatrix);

    float4x4 PreMatrixX = mul(g_OffsetMatrices[In.vBlendIndex.x], g_PreBoneMatrixBuffer[In.vBlendIndex.x].BoneCombinedTransformMatrix);
    float4x4 PreMatrixY = mul(g_OffsetMatrices[In.vBlendIndex.y], g_PreBoneMatrixBuffer[In.vBlendIndex.y].BoneCombinedTransformMatrix);
    float4x4 PreMatrixZ = mul(g_OffsetMatrices[In.vBlendIndex.z], g_PreBoneMatrixBuffer[In.vBlendIndex.z].BoneCombinedTransformMatrix);
    float4x4 PreMatrixW = mul(g_OffsetMatrices[In.vBlendIndex.w], g_PreBoneMatrixBuffer[In.vBlendIndex.w].BoneCombinedTransformMatrix);
    
    matrix BoneMatrix = MatrixX * In.vBlendWeight.x +
        MatrixY * In.vBlendWeight.y +
        MatrixZ * In.vBlendWeight.z +
        MatrixW * In.vBlendWeight.w;
    
    matrix PreBoneMatrix = PreMatrixX * In.vBlendWeight.x +
        PreMatrixY * In.vBlendWeight.y +
        PreMatrixZ * In.vBlendWeight.z +
        PreMatrixW * In.vBlendWeight.w;
   
    /* 스키닝 */
    vector vCurrentPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vPrePosition = mul(vector(In.vPosition, 1.f), PreBoneMatrix);
    
    matrix matWV, matWVP, matOldWV, matOldWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
        
    Out.vPosition = mul(vCurrentPosition, matWVP);
    float4 vNewPos = Out.vPosition;
    
    matOldWV = mul(g_PreWorldMatrix, g_PreViewMatrix);
    matOldWVP = mul(matOldWV, g_ProjMatrix);
    vector vOldPos = mul(vPrePosition, matOldWVP);
    
    float3 vDir = vOldPos.xyz - vNewPos.xyz;
    vector vNormal = normalize(mul(vector(In.vNormal, 0.f), matWV));
    
    float2 fVelocity = (vNewPos.xy / vNewPos.w) - (vOldPos.xy / vOldPos.w);
    
    Out.vDirection.xy = fVelocity * 0.5f;
    Out.vDirection.y *= -1.f;
    
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord) * float4(1.f, 0.5f, 0.5f, 1.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

/* 림라이트 적용 버전. */
PS_OUT PS_MAIN_RIMLIGHT(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse +
        Calc_RimLight(g_fRimLightStrength, g_fRimLightPower, g_vCamPosition, g_vRimLightColor, In.vNormal, In.vWorldPos);;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    //림라이트도 더해서 던져.
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    
    Out.vShadowLightDepth.x = In.vProjPos.z / In.vProjPos.w;
    
    return Out;
}

PS_OUT_MOTIONBLUR PS_MAIN_MOTIONBLUR(PS_IN_MOTIONBLUR In)
{
    //노말맵은 안..쓰지.
    PS_OUT_MOTIONBLUR Out;
    Out.vDirection.xy = In.vDirection.xy;
    Out.vDirection.z = 0.f;
    Out.vDirection.w = 1.f;
    
    return Out;
}

PS_OUT PS_MAIN_MA_MouthInner_Inst(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
       
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord) * float4(1.f, 0.5f, 0.5f, 1.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_M_MikeEyeBlend_Inst(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord) * float4(1.f, 0.5f, 0.5f, 1.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_M_lacrimal_fluid(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord) * float4(1.f, 0.5f, 0.5f, 1.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_MI_EVE_Head_V02(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
    
    //vMtrlORSS.a = 2.f;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal, 0.0001f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    //Out.vORM = Calc_ORSS(g_ORSSTexture, In.vTexcoord);
    Out.vORM = float4(1.f, 0.5f, 0.f, 0.f);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord) * float4(1.f, 0.5f, 0.5f, 1.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_MI_EyeRefractive1(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord) * float4(1.f, 0.5f, 0.5f, 1.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_MI_EVE_Eyeshadow_Occlusion(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vMtrlDiffuse.a = vMtrlDiffuse.r;
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord) * float4(1.f, 0.5f, 0.5f, 1.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_NewMaterial(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord) * float4(1.f, 0.5f, 0.5f, 1.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_MI_EyeBrow1(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMtrlOpacity = g_OpacityTexture.Sample(DefaultSampler, In.vTexcoord);
    vMtrlDiffuse.a = vMtrlOpacity.r;
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
   
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord) * float4(1.f, 0.5f, 0.5f, 1.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_EyeLight_Inst(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord) * float4(1.f, 0.5f, 0.5f, 1.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_MI_Teeth(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord) * float4(1.f, 0.5f, 0.5f, 1.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_MA_TeethOcculusion_Inst1(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord) * float4(1.f, 0.5f, 0.5f, 1.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    // 0
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    // 1
    pass Shadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = compile gs_5_0 GS_MAIN_SHADOW();
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

    // 림 라이트 켠 버전
    // 2
    pass RimLight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RIMLIGHT();
    }
    // 모션 블러
    // 3
    pass MotionBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_MOTIONBLUR();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MOTIONBLUR();
    }
    
    pass MA_MouthInner_Inst
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MA_MouthInner_Inst();
    }

    pass M_MikeEyeBlend_Inst
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_M_MikeEyeBlend_Inst();
    }

    pass M_lacrimal_fluid
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_M_lacrimal_fluid();
    }

    pass MI_EVE_Head_V02
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MI_EVE_Head_V02();
    }

    pass MI_EyeRefractive1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MI_EyeRefractive1();
    }

    pass MI_EVE_Eyeshadow_Occlusion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MI_EVE_Eyeshadow_Occlusion();
    }

    pass NewMaterial
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NewMaterial();
    }

    pass MI_EyeBrow1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MI_EyeBrow1();
    }

    pass EyeLight_Inst
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EyeLight_Inst();
    }

    pass MI_Teeth
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MI_Teeth();
    }

    pass MA_TeethOcculusion_Inst1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MA_TeethOcculusion_Inst1();
    }
}