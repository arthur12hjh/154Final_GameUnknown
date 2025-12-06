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
    int     iNumInstance;
    int     iPadding;
    int     iPadding2;
}

StructuredBuffer<InstanceVertices> Input : register(t0);
AppendStructuredBuffer<InstanceVertices> g_Out : register(u0);
RWStructuredBuffer<uint> g_CountOut : register(u1);

[numthreads(1024, 1, 1)]
void CS_Main(uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
        uint GI : SV_GroupIndex)
{
    if (iNumInstance <= DTid.x)
        return;
    
    for (int i = 0; i < 6; ++i)
    {
        if (fDistance.x < dot(vFrustomPlane[i], Input[DTid.x].vTranslation))
            return;
    }
    
    g_CountOut[0]++;
    g_Out.Append(Input[DTid.x]);
}