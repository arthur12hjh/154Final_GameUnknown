#include "../../../Client/Bin/ShaderFiles/Client_Shader_Utils.hlsli"
#include "../../../Client/Bin/ShaderFiles/Client_Shader_VtxAnimMesh_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_PreWorldMatrix, g_PreViewMatrix;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_EmissiveTexture;
Texture2D g_ORMTexture;

//림라이트용 변수
vector g_vCamPosition;
float g_fRimLightPower;
float g_fRimLightStrength;
float4 g_vRimLightColor;

/* 메시다 ㅇ영향을 주는 뼈들의 집합*/
matrix g_OffsetMatrices[512];

StructuredBuffer<BoneTransformMatrix> g_BoneMatrixBuffer : register(t16);
StructuredBuffer<BoneTransformMatrix> g_PreBoneMatrixBuffer : register(t17);

/* 정점 쉐이더 : */
/* 정점에 대한 셰이딩 == 정점에 필요한 연산을 수행한다 == 정점의 상태변환(월드, 뷰, 투영) + 추가변환 */
/* 정점의 구성 정보를 수정, 변경한다 */ 

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
   
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vProjPos = Out.vPosition;

    return Out;
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
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);
    Out.vORM = g_ORMTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord) * float4(1.f, 0.5f, 0.5f, 1.f);
    
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
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);
    Out.vORM = g_ORMTexture.Sample(DefaultSampler, In.vTexcoord);
    //림라이트도 더해서 던져.
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord);
    
    return Out;
}

PS_OUT PS_MAIN_EYEMASKING(PS_IN In)
{
    PS_OUT Out;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    
    if (Out.vDiffuse.r <= 0.7f)
        discard;
    
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);
    Out.vORM = g_ORMTexture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    
    Out.vShadowLightDepth.x = In.vProjPos.w / 500.0f;
    
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
        GeometryShader = NULL;
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

    // 눈 마스킹용.
    // 3 
    pass Mask
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EYEMASKING();
    }
    // 모션 블러
    // 4
    pass MotionBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_MOTIONBLUR();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MOTIONBLUR();
    }
}



