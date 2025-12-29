#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

int g_iWinSizeX;
int g_iWinSizeY;
float g_fFar;

float g_fFogStart, g_fFogEnd;
float g_fFogPowerMin, g_fFogPowerMax;
float g_fSkyBoxFogPower;
//fog 지수가 적용되는 분기점. 0~ 1 사이값으로 줘야함.
float g_fFogDistanceValue = 0.5f;
float g_fFogHeightValue = 0.01f;
float g_fFogDensity = 0.01f;

float g_fFogBaseHeight = 1.f;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

Texture3D g_PerlinNoiseTexture;
texture2D g_DepthTexture;
float3 g_vCamPosition;

//뎁스 텍스쳐 기반으로 포그 강도 기록해둘 렌더타겟

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;   
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);   
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

PS_OUT_FOG PS_MAIN_FOG(PS_IN In)
{
    PS_OUT_FOG Out;
    Out.fFogPower = 1.0f;
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    //깊이 기록 되지 않은 녀석들에게 처리해주기 위한 코드
    if (vDepthDesc.r == 0 && vDepthDesc.g == 1 && vDepthDesc.b == 0 && vDepthDesc.a == 0)
    {
        Out.fFogPower = g_fSkyBoxFogPower;
        return Out;
    }
    
    float fViewZ = vDepthDesc.y * g_fFar;
    vector vPosition;

    /* 로컬위치 * 월드 * 뷰 * 투영 / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    /* 로컬위치 * 월드 * 뷰 * 투영  */
    vPosition = vPosition * fViewZ;
    /* 로컬위치 * 월드 * 뷰  */
    vPosition = mul(vPosition, g_ProjMatrixInv);

    Out.fFogPower = clamp((g_fFogEnd - vPosition.z) / (g_fFogEnd - g_fFogStart), g_fFogPowerMin, g_fFogPowerMax);
    
    return Out;
}

PS_OUT_FOG PS_MAIN_VOLUME_FOG(PS_IN In)
{
    PS_OUT_FOG Out;
    vector vDepthDesc = g_DepthTexture.Sample(ClampSampler, In.vTexcoord);

    //스카이박스는 예외처리하고
    if (vDepthDesc.r == 0 && vDepthDesc.g == 1 && vDepthDesc.b == 0 && vDepthDesc.a == 0)
    {
        Out.fFogPower = g_fSkyBoxFogPower;
        return Out;
    }

    //뷰 스페이스 상의 z..?
    float fViewZ = vDepthDesc.y * g_fFar;
    vector vPosition;
    
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    vPosition = vPosition * fViewZ;
    
    // 스크린 스페이스 좌표를 월드 좌표까지 내린다.
    vPosition = mul(vPosition, g_ProjMatrixInv);
    vPosition = mul(vPosition, g_ViewMatrixInv);
   
    // 안개가 높이에 따라 사라지는 속도
    float fFogFallOff = 0.1f;
    // 카메라에서 물체까지의 벡터
    float3 vCamToWorld = (vPosition.xyz - g_vCamPosition);

    // 총 거리
    float fDist = length(fViewZ);
    // 안개가 시작되는 지점 이후의 거리. 음수가 나오면.. 0.001 처리.
    float fFogDist = max(fDist - g_fFogStart, 0.f);
    
    // 카메라 높이에서의 밀도
    float fFogHeightDensity = exp(-fFogFallOff * g_vCamPosition.y);
    // 거리와 카메라 높이 밀도를 곱함. ( 거리기반 안개는 여기서 끝 )
    float fFogDistIntegral = fFogDist * fFogHeightDensity;
    
    // 시선 방향에 따른 높이 변화량
    float fCamToPixelY = vCamToWorld.y * (fFogDist / max(fDist, 0.1f));
    float t = -fFogFallOff * fCamToPixelY;
    float thresholdT = 0.01;
    
    //적분 공식을 통해 안개 수치를 유도해낸다.
    float fFogHeightIntegral = abs(t) > thresholdT ? (1.0 - exp(-t)) / t : 1.0;

    float fFog = exp(-g_fFogDensity * fFogDistIntegral * fFogHeightIntegral);
    
    Out.fFogPower = fFog;
    
    return Out;
}

technique11 DefaultTechnique
{ 
    // idx 0
    pass LinearFog
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FOG();
    }

    pass VolumeFog
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_VOLUME_FOG();
    }
}