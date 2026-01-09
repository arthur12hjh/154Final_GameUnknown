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
Texture2D g_DissolveTexture;

//������Ʈ�� ����
vector g_vCamPosition;
float g_fRimLightPower;
float g_fRimLightStrength;

float4      g_vRimLightColor;
float       g_fRimLightColorRatio;

bool        g_IsPattern;
bool        g_IsPatternNoise;
bool        g_IsFade;

float4      g_vCenterPos;
float       g_fFadeRadius;
float4      g_vMeshColor;

//������� ����
float g_fDeadTime;
float g_fFar;

/* �޽ô� �������� �ִ� ������ ����*/
matrix g_OffsetMatrices[512];

// for cascade 
matrix g_LightViewMatrix[CASCADE_LEVEL];
matrix g_LightProjMatrix[CASCADE_LEVEL];

StructuredBuffer<BoneTransformMatrix> g_BoneMatrixBuffer : register(t16);
StructuredBuffer<BoneTransformMatrix> g_PreBoneMatrixBuffer : register(t17);

/* ���� ���̴� : */
/* ������ ���� ���̵� == ������ �ʿ��� ������ �����Ѵ� == ������ ���º�ȯ(����, ��, ����) + �߰���ȯ */
/* ������ ���� ������ ����, �����Ѵ� */ 

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float fWeightSum = (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z + In.vBlendWeight.w);
    
    float fWeightX = 1;
    float fWeightY = 0;
    float fWeightZ = 0;
    float fWeightW = 0;
    
    if (fWeightSum > 1e-6f)
    {
        fWeightX = In.vBlendWeight.x / fWeightSum;
        fWeightY = In.vBlendWeight.y / fWeightSum;
        fWeightZ = In.vBlendWeight.z / fWeightSum;
        fWeightW = In.vBlendWeight.w / fWeightSum;
    }
          
    float4x4 MatrixX = mul(g_OffsetMatrices[In.vBlendIndex.x], g_BoneMatrixBuffer[In.vBlendIndex.x].BoneCombinedTransformMatrix);
    float4x4 MatrixY = mul(g_OffsetMatrices[In.vBlendIndex.y], g_BoneMatrixBuffer[In.vBlendIndex.y].BoneCombinedTransformMatrix);
    float4x4 MatrixZ = mul(g_OffsetMatrices[In.vBlendIndex.z], g_BoneMatrixBuffer[In.vBlendIndex.z].BoneCombinedTransformMatrix);
    float4x4 MatrixW = mul(g_OffsetMatrices[In.vBlendIndex.w], g_BoneMatrixBuffer[In.vBlendIndex.w].BoneCombinedTransformMatrix);
    
    matrix BoneMatrix = MatrixX * fWeightX  +
                        MatrixY * fWeightY +
                        MatrixZ * fWeightZ +
                        MatrixW * fWeightW;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    float3 vSkinnedNormal = mul(float4(In.vNormal, 0.f), BoneMatrix).xyz;
    float3 vSkinnedTangent = mul(float4(In.vTangent, 0.f), BoneMatrix).xyz;
    float3 vSkinnedBinorm = mul(float4(In.vBinormal, 0.f), BoneMatrix).xyz;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    /* Out.vPosition.xy => �þ߰��� �ִ� ������ 90�� �����ش� */ 
    /* Out.vPosition.z => n~f���̿� �ִ� ������ z�� 0 ~ f�� �ٲ��ش�. */   
    Out.vNormal = float4(normalize(mul(float4(vSkinnedNormal, 0.f), g_WorldMatrix).xyz), 0.f);
    Out.vTangent = float4(normalize(mul(float4(vSkinnedTangent, 0.f), g_WorldMatrix).xyz), 0.f);
    Out.vBinormal = float4(normalize(mul(float4(vSkinnedBinorm, 0.f), g_WorldMatrix).xyz), 0.f);
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;
    
    float fWeightSum = (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z + In.vBlendWeight.w);
    
    float fWeightX = 1;
    float fWeightY = 0;
    float fWeightZ = 0;
    float fWeightW = 0;
    
    if (fWeightSum > 1e-6f)
    {
        fWeightX = In.vBlendWeight.x / fWeightSum;
        fWeightY = In.vBlendWeight.y / fWeightSum;
        fWeightZ = In.vBlendWeight.z / fWeightSum;
        fWeightW = In.vBlendWeight.w / fWeightSum;
    }    
    
    float4x4 MatrixX = mul(g_OffsetMatrices[In.vBlendIndex.x], g_BoneMatrixBuffer[In.vBlendIndex.x].BoneCombinedTransformMatrix);
    float4x4 MatrixY = mul(g_OffsetMatrices[In.vBlendIndex.y], g_BoneMatrixBuffer[In.vBlendIndex.y].BoneCombinedTransformMatrix);
    float4x4 MatrixZ = mul(g_OffsetMatrices[In.vBlendIndex.z], g_BoneMatrixBuffer[In.vBlendIndex.z].BoneCombinedTransformMatrix);
    float4x4 MatrixW = mul(g_OffsetMatrices[In.vBlendIndex.w], g_BoneMatrixBuffer[In.vBlendIndex.w].BoneCombinedTransformMatrix);

    matrix BoneMatrix =
        MatrixX * fWeightX +
        MatrixY * fWeightY +
        MatrixZ * fWeightZ +
        MatrixW * fWeightW;

    float4 vSkinnedLocal = mul(float4(In.vPosition, 1.f), BoneMatrix);

    // ���������
    Out.vPosition = mul(vSkinnedLocal, g_WorldMatrix);

    return Out;
}

VS_OUT_MOTIONBLUR VS_MAIN_MOTIONBLUR(VS_IN In)
{
    VS_OUT_MOTIONBLUR Out;
    
    float fWeightSum = (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z + In.vBlendWeight.w);
    
    float fWeightX = 1;
    float fWeightY = 0;
    float fWeightZ = 0;
    float fWeightW = 0;
    
    if (fWeightSum > 1e-6f)
    {
        fWeightX = In.vBlendWeight.x / fWeightSum;
        fWeightY = In.vBlendWeight.y / fWeightSum;
        fWeightZ = In.vBlendWeight.z / fWeightSum;
        fWeightW = In.vBlendWeight.w / fWeightSum;
    }
    
    // CPU�� ������ ��� ����
    float4x4 MatrixX = mul(g_OffsetMatrices[In.vBlendIndex.x], g_BoneMatrixBuffer[In.vBlendIndex.x].BoneCombinedTransformMatrix);
    float4x4 MatrixY = mul(g_OffsetMatrices[In.vBlendIndex.y], g_BoneMatrixBuffer[In.vBlendIndex.y].BoneCombinedTransformMatrix);
    float4x4 MatrixZ = mul(g_OffsetMatrices[In.vBlendIndex.z], g_BoneMatrixBuffer[In.vBlendIndex.z].BoneCombinedTransformMatrix);
    float4x4 MatrixW = mul(g_OffsetMatrices[In.vBlendIndex.w], g_BoneMatrixBuffer[In.vBlendIndex.w].BoneCombinedTransformMatrix);

    float4x4 PreMatrixX = mul(g_OffsetMatrices[In.vBlendIndex.x], g_PreBoneMatrixBuffer[In.vBlendIndex.x].BoneCombinedTransformMatrix);
    float4x4 PreMatrixY = mul(g_OffsetMatrices[In.vBlendIndex.y], g_PreBoneMatrixBuffer[In.vBlendIndex.y].BoneCombinedTransformMatrix);
    float4x4 PreMatrixZ = mul(g_OffsetMatrices[In.vBlendIndex.z], g_PreBoneMatrixBuffer[In.vBlendIndex.z].BoneCombinedTransformMatrix);
    float4x4 PreMatrixW = mul(g_OffsetMatrices[In.vBlendIndex.w], g_PreBoneMatrixBuffer[In.vBlendIndex.w].BoneCombinedTransformMatrix);
    
    matrix BoneMatrix = MatrixX * fWeightX +
                        MatrixY * fWeightY +
                        MatrixZ * fWeightZ +
                        MatrixW * fWeightW;
    
    matrix PreBoneMatrix =  PreMatrixX * fWeightX +
                            PreMatrixY * fWeightY +
                            PreMatrixZ * fWeightZ +
                            PreMatrixW * fWeightW;
   
    /* ��Ű�� */
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
    
    float4 curVS = mul(vCurrentPosition, matWV);
    float4 preVS = mul(vPrePosition, matOldWV);

    float3 vDirVS = curVS.xyz - preVS.xyz;
    float3 vNormalVS = normalize(mul(float4(In.vNormal, 0.f), matWV).xyz);

    float a = dot(normalize(vDirVS), vNormalVS);
    
    //지나간 경로에 벨로시티 찍어줘서, 캐릭터는 안묻어나오게 한다.
    if(a < 0.f)
        Out.vPosition = vOldPos;
    else
        Out.vPosition = vNewPos;
   
    float2 vVelocity = vNewPos.xy / vNewPos.w - vOldPos.xy / vOldPos.w;
    Out.vDirection.xy = vVelocity * 0.5f;
    Out.vDirection.y *= -1.f;
   
    Out.vDirection.z = Out.vPosition.z / Out.vPosition.w;
    Out.vDirection.w = 0.f;
    
    return Out;
}

[maxvertexcount(CASCADE_LEVEL * 3)]
void GS_MAIN_SHADOW(triangle VS_OUT_SHADOW InTri[3], inout TriangleStream<GS_OUT_SHADOW> OutStream)
{
    //CASCADE LEVEL ��ȸ�ϸ鼭 �ѹ��� ��� �ϱ�
    [unroll]
    for (uint iCount = 0; iCount < CASCADE_LEVEL; ++iCount)
    {
        matrix matLightVP = mul(g_LightViewMatrix[iCount], g_LightProjMatrix[iCount]);

        GS_OUT_SHADOW Out;

        // ���� 3���� �ﰢ�� �ϳ��� 
        // ����ؼ� �������ֱ�
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

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    if (g_IsPattern)
        Out.vDiffuse = g_vMeshColor;
    else
        Out.vDiffuse = vMtrlDiffuse;
    
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

/* ������Ʈ ���� ����. */
PS_OUT PS_MAIN_RIMLIGHT(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse +
        Calc_RimLight(g_fRimLightStrength, g_fRimLightPower, g_vCamPosition, g_vRimLightColor, In.vNormal, In.vWorldPos);
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    //������Ʈ�� ���ؼ� ����.
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_EYEMASKING(PS_IN In)
{
    PS_OUT Out;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    
    if(Out.vDiffuse.r <= 0.7f)
        discard;
    
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal, 0.001f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = g_ORMTexture.Sample(DefaultSampler, In.vTexcoord);
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
    //�븻���� ��..����.
    PS_OUT_MOTIONBLUR Out;
    Out.vDirection = In.vDirection;
    
    return Out;
}

PS_OUT PS_DISSOLVE(PS_IN In)
{
    PS_OUT Out;
    float3 dir = normalize(float3(0, -1, 0));
    float h = dot(normalize(In.vWorldPos.xyz - g_WorldMatrix._41_42_43), dir);
    
    h = h * 0.5 + 0.5;

    float noise = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord).r;
    h += noise * 0.2;

    if (h * 4 + 0.6 < g_fDeadTime)
        discard;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    

    if (h * 4 < g_fDeadTime)
    {
        noise = min(h * 4 + 0.3 - g_fDeadTime, 1);
        Out.vDiffuse.rgb *= noise;
    }
    
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;

}

PS_OUT PS_GORILLA_DISSOLVE(PS_IN In)
{
    PS_OUT Out;
    
    float3 dir = normalize(float3(0, -1, 0));
    float h = dot(normalize(In.vWorldPos.xyz - g_WorldMatrix._41_42_43), dir);
    
    h = h * 0.5 + 0.5;

    float dissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord).r;
    h += dissolve * 0.2;
    h *= 0.5;

    if (h + 2.1f < g_fDeadTime)
        discard;
    
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    if (dissolve < g_fDeadTime)
    {
        dissolve = min(dissolve + 0.3 - g_fDeadTime, 1);
        Out.vDiffuse.rgb *= dissolve;
    }
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_ORSS(PS_IN In)
{
    PS_OUT Out;
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORSS(g_ORSSTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_SCARLET_ATK_COLOR(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    float4 vColor;
    
    vColor = true == g_IsPattern ? g_vMeshColor : g_vMtrlDiffuse;
    vColor += Calc_RimLight(g_fRimLightStrength, g_fRimLightPower, g_vCamPosition, g_vRimLightColor, In.vNormal, In.vWorldPos);
    if (g_IsPatternNoise && 1 <= g_fRimLightColorRatio)
        discard;
    
    //{
    //    float3 vPoint = In.vWorldPos.xyz;
    //    float3 vTargetPoint = g_vCenterPos.xyz;
        
    //    vTargetPoint.z = vPoint.z = 0.f;
    //    float fLength = length(vPoint - vTargetPoint);
    //    if (g_IsFade)
    //    {
    //         // �ۿ��� ��������
    //        if (fLength >= g_fFadeRadius * ( 0.2f - g_fDeadTime / 0.2f))
    //            discard;
    //    }
    //    else
    //    {
    //        // ���ʿ��� ������
    //        if (fLength <= g_fFadeRadius * (g_fDeadTime / 0.2f))
    //            discard;
    //    }
    //}
    
    Out.vDiffuse = vColor;
    Out.vNormal = Calc_Normal(g_NormalTexture, In.vTexcoord, In.vNormal, In.vTangent, In.vBinormal);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    
    //������Ʈ�� ���ؼ� ����.
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_NONE_NORMAL(PS_IN In)
{
    PS_OUT Out;
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    if (g_IsPattern)
        Out.vDiffuse = g_vMeshColor;
    else
        Out.vDiffuse = vMtrlDiffuse;
    
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = Calc_Emissive(g_EmissiveTexture, Out.vDiffuse, In.vTexcoord);
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_HAIR_ALPHA_CUT(PS_IN In)
{
    PS_OUT Out;
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    //vector vMtrlOpacity = g_OpacityTexture.Sample(DefaultSampler, In.vTexcoord);
    //vMtrlDiffuse.rgb *= vMtrlOpacity.r;
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    if (g_IsPattern)
        Out.vDiffuse = g_vMeshColor;
    else
        Out.vDiffuse = vMtrlDiffuse;
    
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_IsMaskingDepthB == true ? 1.f : 0.f, 0.0f);
    Out.vORM = Calc_ORM(g_ORMTexture, In.vTexcoord);
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    // 0
    pass Default
    {
        //SetRasterizerState(RS_Default);
        SetRasterizerState(RS_Cull_None);
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

    // �� ����Ʈ �� ����
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

    // �� ����ŷ��.
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
    // ��� ����
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
    // ���� ������
    // 5
    pass Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE();
    }
    // 6
    pass ORSS
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ORSS();
    }
    // 7
    pass GorillaDissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GORILLA_DISSOLVE();
    }
    // 8 Scarlet Change Body Color
    pass ScarletColorPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SCARLET_ATK_COLOR();
    }
    // idx 9
    pass NonNormal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NONE_NORMAL();
    }
    // idx 10 Hair Alpha Cutting
    pass HairAlphaCut
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HAIR_ALPHA_CUT();
    }
}



