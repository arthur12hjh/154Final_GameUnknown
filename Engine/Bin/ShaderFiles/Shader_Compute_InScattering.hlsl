#include "Shader_Compute_VolumetricCommon.hlsl"

RWTexture3D<float4> FrustumVolume : register(u0);
Texture3D<float> g_Noise3D : register(t0);
SamplerState g_NoiseSamp : register(s0);


/*
- global parameter
FrustumVolume : 볼륨 텍스쳐
CameraPos : 카메라 위치
Density : 균등한 밀도
HemisphereUpperColor : Ambient로 사용하는 반구 조명 색상
Intensity : 빛 강도 조절을 위한 변수
*/

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadId)
{
    float4 HemisphereUpperColor = float4(0.4, 0.45, 0.5, 0.1);

    uint3 dims;
    FrustumVolume.GetDimensions(dims.x, dims.y, dims.z);

    if (all(DTid < dims))
    {   
        /*
                float3 jitter = HALTON_SEQUENCE[(DTid.x + DTid.y * 2) % MAX_HALTON_SEQUENCE];
        jitter.xyz -= 0.5f;
        jitter.z = 0.f;
        */
        float3 jitter = 0.f;
        
        float3 worldPosition = ConvertThreadIdToWorldPosition(DTid, dims, jitter);
        float3 toCamera = normalize(CameraPos.xyz - worldPosition);

        float3 noiseUVW = (worldPosition - NoiseAnchor.xyz) * NoiseScale;
        
        // 바람 방향(월드 기준) + 속도
        float3 windDir = normalize(float3(1.0f, 0.0f, 0.2f)); // 원하는 방향
        float windSpeed = 0.1f; // 0.01~0.08

        noiseUVW += windDir * (windSpeed * NoiseTime);

        float n01 = g_Noise3D.SampleLevel(g_NoiseSamp, noiseUVW, 0).r; // 0..1

        float threshold = 0.55f; // 0.45~0.70
        float softness = 0.12f; // 0.05~0.20
        float blob = smoothstep(threshold - softness, threshold + softness, n01); // 0..1
        float localDensity = Density * blob;
        
        localDensity = max(localDensity, 0.0f);

        float3 lighting = HemisphereUpperColor.rgb * HemisphereUpperColor.a;

        float3 vLightDirection = normalize(LightDirection.xyz);
        float fPhaseFunction = HenyeyGreensteinPhaseFunction(vLightDirection, toCamera, AsymmetryParameterG);
        lighting += LightColor * fPhaseFunction;

        float4 curScattering = float4(lighting * Intensity * localDensity, localDensity);
        FrustumVolume[DTid] = curScattering;
    }
}