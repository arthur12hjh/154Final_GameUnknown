#include "Shader_Deferred_Defines.hlsli"
#include "Shader_Deferred_Functions.hlsli"

float g_fFar;
int g_iWinSizeX;
int g_iWinSizeY;
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ProjMatrixInv, g_ViewMatrixInv;

Texture3D g_VolumetricFogTexture;
texture2D g_DepthTexture;
vector g_vCamPosition;

float g_fDepthPackExponent;
float g_fNearPlaneDist;
float g_fFarPlaneDist;

float ConvertDepthToNdcZ(float depth)
{
    float depthPackExponent = g_fDepthPackExponent;
    float nearPlaneDist = g_fNearPlaneDist;
    float farPlaneDist = g_fFarPlaneDist;

    return pow(saturate((depth - nearPlaneDist) / (farPlaneDist - nearPlaneDist)), 1 / depthPackExponent);
}

float3 GetWorldRayDir(float2 vTexcoord)
{
    float2 vNdc;
    vNdc.x = vTexcoord.x * 2.0f - 1.0f;
    vNdc.y = vTexcoord.y * -2.0f + 1.0f;

    // far plane clip pos
    float4 vClip = float4(vNdc, 1.0f, 1.0f);

    // clip -> view
    float4 vView = mul(vClip, g_ProjMatrixInv);
    vView.xyz /= max(vView.w, 1e-6f);

    float3 vDirView = normalize(vView.xyz);

    // view dir -> world dir
    float3 vDirWorld = mul(float4(vDirView, 0.0f), g_ViewMatrixInv).xyz;
    return normalize(vDirWorld);
}


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

PS_OUT_VOLUME_FOG PS_MAIN_VOLUME_FOG(PS_IN In)
{
    PS_OUT_VOLUME_FOG Out;

    float4 vDepth = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);

    // ===== 스카이박스 최소 처리 =====
    if (vDepth.r == 0 && vDepth.g == 1 && vDepth.b == 0 && vDepth.a == 0)
    {
        // 머뭇거릴틈이없다
        float fSkyFogZBase = 0.985f; // far 근처
        float fSkyFogSpread0 = 0.003f; // 상단 하늘
        float fSkyFogSpread1 = 0.040f; // 수평선 근처
        float fHorizonStart = 0.10f;
        float fHorizonEnd = 0.85f;

        // horizon 마스크 (수평선 근처일수록 1)
        float3 vRayDirWorld = GetWorldRayDir(In.vTexcoord);
        float fHorizon = saturate(1.0f - abs(vRayDirWorld.y));
        float fMask = smoothstep(fHorizonStart, fHorizonEnd, fHorizon);

        // far z를 3탭 분산 샘플 (수평선일수록 spread 증가)
        float fSpread = lerp(fSkyFogSpread0, fSkyFogSpread1, fHorizon);

        float z0 = saturate(fSkyFogZBase - fSpread);
        float z1 = saturate(fSkyFogZBase);
        float z2 = saturate(fSkyFogZBase + fSpread);

        float4 s0 = g_VolumetricFogTexture.Sample(Noise3DSampler, float3(In.vTexcoord, z0));
        float4 s1 = g_VolumetricFogTexture.Sample(Noise3DSampler, float3(In.vTexcoord, z1));
        float4 s2 = g_VolumetricFogTexture.Sample(Noise3DSampler, float3(In.vTexcoord, z2));

        float4 vFog = (s0 + s1 + s2) * (1.0f / 3.0f);

        // 스카이는 마스크로만 개입(위쪽 하늘은 거의 0, 수평선 쪽만)
        float3 vScattering = vFog.rgb * fMask;
        float fTransmittance = lerp(1.0f, vFog.a, fMask);

        Out.vVolumeFogDesc = float4(vScattering, fTransmittance);
        return Out;
    }

    // 기존 경로
    float fViewDepth = vDepth.y * g_fFar;
    vector vViewPosition = GetViewPosition(g_DepthTexture, g_fFar, In.vTexcoord, g_ProjMatrixInv);

    Out.vVolumeFogDesc =
        g_VolumetricFogTexture.Sample(Noise3DSampler, float3(In.vTexcoord, ConvertDepthToNdcZ(fViewDepth)));

    return Out;
        /*
            
    PS_OUT_VOLUME_FOG Out;


    float4 vDepth = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);

    if (vDepth.r == 0 && vDepth.g == 1 && vDepth.b == 0 && vDepth.a == 0)
    {
        Out.vVolumeFogDesc = float4(0.f, 0.f, 0.f, 1.f);
        return Out;
    }
    
    float fViewDepth = vDepth.y * g_fFar;
    vector vViewPosition = GetViewPosition(g_DepthTexture, g_fFar, In.vTexcoord, g_ProjMatrixInv);
    
    Out.vVolumeFogDesc = g_VolumetricFogTexture.Sample(Noise3DSampler, float3(In.vTexcoord, ConvertDepthToNdcZ(fViewDepth)));

    
    return Out;
        */
}

technique11 DefaultTechnique
{
    pass VolumeFog
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0, 0, 0, 0), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_VOLUME_FOG();
    }
}