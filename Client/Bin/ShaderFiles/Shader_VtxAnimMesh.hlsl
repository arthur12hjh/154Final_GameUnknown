#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_PreWorldMatrix, g_PreViewMatrix;
Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_EmissiveTexture;
Texture2D g_ORMTexture;

vector g_vCamPosition;
/* 메시다 ㅇ영향을 주는 뼈들의 집합*/
matrix g_OffsetMatrices[512];

bool g_IsMotionBlur;

StructuredBuffer<BoneTransformMatrix> g_BoneMatrixBuffer : register(t16);

/* 정점 쉐이더 : */
/* 정점에 대한 셰이딩 == 정점에 필요한 연산을 수행한다 == 정점의 상태변환(월드, 뷰, 투영) + 추가변환 */
/* 정점의 구성 정보를 수정, 변경한다 */ 

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
          
// CPU와 동일한 행렬 순서
    float4x4 MatrixX = mul(g_OffsetMatrices[In.vBlendIndex.x], g_BoneMatrixBuffer[In.vBlendIndex.x].BoneCombinedTransformMatrix);
    float4x4 MatrixY = mul(g_OffsetMatrices[In.vBlendIndex.y], g_BoneMatrixBuffer[In.vBlendIndex.y].BoneCombinedTransformMatrix);
    float4x4 MatrixZ = mul(g_OffsetMatrices[In.vBlendIndex.z], g_BoneMatrixBuffer[In.vBlendIndex.z].BoneCombinedTransformMatrix);
    float4x4 MatrixW = mul(g_OffsetMatrices[In.vBlendIndex.w], g_BoneMatrixBuffer[In.vBlendIndex.w].BoneCombinedTransformMatrix);

    
    //matrix SkinnedMatrix = mul(g_BoneMatrixBuffer[In.vBlendIndex.x].BoneCombinedTransformMatrix, g_OffsetMatrices[In.vBlendIndex.x]);
    //matrix SkinnedMatrix = g_OffsetMatrices[In.vBlendIndex.x];
    
    //matrix BoneMatrix = g_OffsetMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
    //    g_OffsetMatrices[In.vBdlendIndex.y] * In.vBlendWeight.y +
    //    g_OffsetMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
    //    g_OffsetMatrices[In.vBlendIndex.w] * fWeightW;
    
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

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    // CPU와 동일한 행렬 순서
    float4x4 MatrixX = mul(g_OffsetMatrices[In.vBlendIndex.x], g_BoneMatrixBuffer[In.vBlendIndex.x].BoneCombinedTransformMatrix);
    float4x4 MatrixY = mul(g_OffsetMatrices[In.vBlendIndex.y], g_BoneMatrixBuffer[In.vBlendIndex.y].BoneCombinedTransformMatrix);
    float4x4 MatrixZ = mul(g_OffsetMatrices[In.vBlendIndex.z], g_BoneMatrixBuffer[In.vBlendIndex.z].BoneCombinedTransformMatrix);
    float4x4 MatrixW = mul(g_OffsetMatrices[In.vBlendIndex.w], g_BoneMatrixBuffer[In.vBlendIndex.w].BoneCombinedTransformMatrix);

    
    //matrix SkinnedMatrix = mul(g_BoneMatrixBuffer[In.vBlendIndex.x].BoneCombinedTransformMatrix, g_OffsetMatrices[In.vBlendIndex.x]);
    //matrix SkinnedMatrix = g_OffsetMatrices[In.vBlendIndex.x];
    
    //matrix BoneMatrix = g_OffsetMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
    //    g_OffsetMatrices[In.vBdlendIndex.y] * In.vBlendWeight.y +
    //    g_OffsetMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
    //    g_OffsetMatrices[In.vBlendIndex.w] * fWeightW;
    
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

struct VS_OUT_MOTIONBLUR
{
    float4 vPosition : SV_POSITION;
    float4 vDirection : TEXCOORD0;
};

VS_OUT_MOTIONBLUR VS_MAIN_MOTIONBLUR(VS_IN In)
{
    VS_OUT_MOTIONBLUR Out;
    
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
    
    matrix matWV, matWVP, matOldWV, matOldWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
        
    Out.vPosition = mul(vPosition, matWVP);
    float4 vNewPos = Out.vPosition;
    
    matOldWV = mul(g_PreWorldMatrix, g_PreViewMatrix);
    matOldWVP = mul(matOldWV, g_ProjMatrix);
    vector vOldPos = mul(vPosition, matOldWVP);
    
    //화면상의 이동량과, 뷰 스페이스 상의 노멀을 연산하여 앞, 뒤 움직임 판정.
    float3 vDir = vOldPos.xyz - vNewPos.xyz;
    vector vNormal = normalize(mul(vector(In.vNormal, 0.f), matWV));
    
    // 잔상 남기는 (공간째 블러하는 ) 코드. 근데 너무 과해서 일단 뺌..
    //float a = dot(normalize(vDir), vNormal.xyz);
    
    //if (a < 0.f)
    //    Out.vPosition = vOldPos;
    //else
    //    Out.vPosition = vNewPos; 
    
    float2 fVelocity = (vNewPos.xy / vNewPos.w) - (vOldPos.xy / vOldPos.w);
    Out.vDirection.xy = fVelocity * 0.5f;
    Out.vDirection.y *= -1.f;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vORM : SV_Target3;
};

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    Out.vDiffuse += Out.vDiffuse * g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);
    Out.vORM = g_ORMTexture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

/* 림라이트 적용 버전. */
PS_OUT PS_MAIN_RIMLIGHT(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    Out.vDiffuse += Out.vDiffuse * g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);

    /* 림라이트 구현부. 수정 필요. */
    vector vPosToCam = normalize(g_vCamPosition - In.vWorldPos);
    float fStrength = 2.f;
    float fPower = 3.f;
    float4 vColor = float4(1.f, 1.f, 1.f, 1.f);
    vector vRimLight = (1 - dot(normalize(In.vNormal), vPosToCam));
    
    vRimLight = pow(vRimLight, fPower) * fStrength * vColor;
    
    Out.vDiffuse += vRimLight;

    return Out;
}

// 림라이트 켜기 + 이미시브 끄기.
PS_OUT PS_MAIN_MS_TEST(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
        discard;
   
    Out.vDiffuse = vMtrlDiffuse;
    //Out.vDiffuse += Out.vDiffuse * g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
   
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);

    /* 림라이트 구현부. 수정 필요. */
    vector vPosToCam = normalize(g_vCamPosition - In.vWorldPos);
    float fStrength = 2.f;
    float fPower = 3.f;
    float4 vColor = float4(1.f, 1.f, 1.f, 1.f);
    vector vRimLight = (1 - dot(normalize(In.vNormal), vPosToCam));
    
    vRimLight = pow(vRimLight, fPower) * fStrength * vColor;
    
    Out.vDiffuse += vRimLight;

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
    
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.0f, 0.0f);
    Out.vORM = g_ORMTexture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

struct PS_IN_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

struct PS_OUT_SHADOW
{
    float4 vShadowLightDepth : SV_TARGET0;
};


PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    
    Out.vShadowLightDepth.x = In.vProjPos.w / 500.0f;
    
    return Out;
}

struct PS_IN_MOTIONBLUR
{
    float4 vPosition : SV_POSITION;
    float4 vDirection : TEXCOORD0;
};

struct PS_OUT_MOTIONBLUR
{
    float4 vDirection : SV_TARGET0;
};

PS_OUT_MOTIONBLUR PS_MAIN_MOTIONBLUR(PS_IN_MOTIONBLUR In)
{
    //노말맵은 안..쓰지.
    PS_OUT_MOTIONBLUR Out;
    Out.vDirection.xy = In.vDirection.xy;
    Out.vDirection.z = 1.f;
    
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