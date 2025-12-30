static const float PI = 3.141592657f;
static const float EPSILON = 1e-6f;

static const int MAX_HALTON_SEQUENCE = 16;

static const float3 HALTON_SEQUENCE[MAX_HALTON_SEQUENCE] =
{
    float3(0.5, 0.333333, 0.2),
	float3(0.25, 0.666667, 0.4),
	float3(0.75, 0.111111, 0.6),
	float3(0.125, 0.444444, 0.8),
	float3(0.625, 0.777778, 0.04),
	float3(0.375, 0.222222, 0.24),
	float3(0.875, 0.555556, 0.44),
	float3(0.0625, 0.888889, 0.64),
	float3(0.5625, 0.037037, 0.84),
	float3(0.3125, 0.37037, 0.08),
	float3(0.8125, 0.703704, 0.28),
	float3(0.1875, 0.148148, 0.48),
	float3(0.6875, 0.481482, 0.68),
	float3(0.4375, 0.814815, 0.88),
	float3(0.9375, 0.259259, 0.12),
	float3(0.03125, 0.592593, 0.32)
};

struct VolumetricFogParameter
{
    float Exposure;
    float DepthPackExponent;
    float NearPlaneDist;
    float FarPlaneDist;
};

cbuffer InscatteringParameters : register(b0)
{
    float AsymmetryParameterG;
    float Density;
    float Intensity;
    float fStartPos;
    
    float NoiseTime;
    float NoiseScale;
    float NoiseStrength;
    float NoiseContrast;
    
    
    row_major matrix g_ViewMatrix;
    row_major matrix g_ProjMatrix;
   
    row_major matrix g_PreViewMatrix;
    row_major matrix g_PreProjMatrix;
    
    row_major matrix g_InvProjectionMatrix;
    row_major matrix g_InvViewMatrix;
    
    float4 CameraPos;
    float4 LightDirection;
    float4 LightColor;
    float4 NoiseAnchor;
    float4 FogAmbient;
}

cbuffer VolumetricFogParameterBuffer : register(b1)
{
    VolumetricFogParameter VolumetricFogParam;
}

float HenyeyGreensteinPhaseFunction(float3 wi, float3 wo, float g)
{
    float cosTheta = dot(wi, wo);
    float g2 = g * g;
    float denom = pow(1.f + g2 - 2.f * g * cosTheta, 3.f / 2.f);
    return (1.f / (4.f * PI)) * ((1.f - g2) / max(denom, EPSILON));
}

float3 ConvertThreadIdToNdc(uint3 id, uint3 dims)
{
    float3 ndc = id;
    ndc += 0.5f;
    ndc *= float3(2.f / dims.x, -2.f / dims.y, 1.f / dims.z);
    ndc += float3(-1.f, 1.f, 0.f);
    return ndc;
}

float3 ConvertThreadIdToNdc(uint3 id, uint3 dims, float3 jitter)
{
    float3 ndc = id;
    ndc += 0.5f;
    ndc += jitter;
    ndc *= float3(2.f / dims.x, -2.f / dims.y, 1.f / dims.z);
    ndc += float3(-1.f, 1.f, 0.f);
    return ndc;
}

float ConvertNdcZToDepth(float ndcZ)
{
    float depthPackExponent = VolumetricFogParam.DepthPackExponent;
    float nearPlaneDist = VolumetricFogParam.NearPlaneDist;
    float farPlaneDist = VolumetricFogParam.FarPlaneDist;

    return pow(ndcZ, depthPackExponent) * (farPlaneDist - nearPlaneDist) + nearPlaneDist;
}

float ConvertDepthToNdcZ(float depth)
{
    float depthPackExponent = VolumetricFogParam.DepthPackExponent;
    float nearPlaneDist = VolumetricFogParam.NearPlaneDist;
    float farPlaneDist = VolumetricFogParam.FarPlaneDist;

    return pow(saturate((depth - nearPlaneDist) / (farPlaneDist - nearPlaneDist)), 1 / depthPackExponent);
}

float3 ConvertToWorldPosition(float3 ndc, float depth)
{
	// view ray
    float4 viewRay = mul(float4(ndc, 1.f), g_InvProjectionMatrix);
    viewRay /= viewRay.w;
    viewRay /= viewRay.z;

	// ndc -> world position
    float4 worldPosition = mul(float4(viewRay.xyz * depth, 1.f), g_InvViewMatrix);

    return worldPosition.xyz;
}

float3 ConvertThreadIdToWorldPosition(uint3 id, uint3 dims)
{
	// id -> ndc
    float3 ndc = ConvertThreadIdToNdc(id, dims);
    float depth = ConvertNdcZToDepth(ndc.z);

    return ConvertToWorldPosition(ndc, depth);
}

float3 ConvertThreadIdToWorldPosition(uint3 id, uint3 dims, float3 jitter)
{
	// id -> ndc
    float3 ndc = ConvertThreadIdToNdc(id, dims, jitter);
    float depth = ConvertNdcZToDepth(ndc.z);

    return ConvertToWorldPosition(ndc, depth);
}

float3 ConvertWorldPositionToUV(float3 worldPosition)
{
    float4 pos = mul(float4(worldPosition, 1.f), g_ViewMatrix * g_ProjMatrix);

    float3 uv = float3(pos.xy / pos.w, ConvertDepthToNdcZ(pos.w));
    uv.xy *= float2(0.5f, -0.5f);
    uv.xy += 0.5f;

    return uv;
}

float SliceTickness(float ndcZ, uint dimZ)
{
    return ConvertNdcZToDepth(ndcZ + 1.f / float(dimZ)) - ConvertNdcZToDepth(ndcZ);
}
