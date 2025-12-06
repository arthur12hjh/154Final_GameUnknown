struct InstanceVertices
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
};

cbuffer ParticleBuffer : register(b0)
{
    float4  vFrustomPlane[6];
    float   fDistance;
}

StructuredBuffer<InstanceVertices> Input : register(t0);
AppendStructuredBuffer<InstanceVertices> g_Out : register(u0);

[numthreads(1024, 1, 1)]
void CS_Main(uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
        uint GI : SV_GroupIndex)
{
    bool bIsFrustom = true;
    for (int i = 0; i < 6; ++i)
    {
        if (fDistance < dot(vFrustomPlane[i], Input[DTid.x].vTranslation))
        {
            bIsFrustom = false;
            break;
        }
    }
    
    if (bIsFrustom)
        g_Out.Append(Input[DTid.x]);
}