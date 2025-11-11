struct ParticleVertices
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;

    float2 vLifeTime;
    float2 vfSpeed;
};

cbuffer ParticleBuffer : register(b0)
{
    float4 vfPivot;
    float2 vfTimeDelta;
    int2 viLoopAndCount;
}


StructuredBuffer<ParticleVertices> Input : register(t0);
RWStructuredBuffer<ParticleVertices> g_Out : register(u0);

[numthreads(256, 1, 1)]
void CS(uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
           uint GI : SV_GroupIndex)
{
    if (DTid.x >= viLoopAndCount.y)
        return;
    g_Out[DTid.x].vRight = Input[DTid.x].vRight;
    g_Out[DTid.x].vUp = Input[DTid.x].vUp;
    g_Out[DTid.x].vLook = Input[DTid.x].vLook;
    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation;
    g_Out[DTid.x].vTranslation.y -= Input[DTid.x].vfSpeed.x * vfTimeDelta.x;
    g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
    g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;

    if (1 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
    {
        g_Out[DTid.x].vTranslation.y -= Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x;
        g_Out[DTid.x].vLifeTime.x = 0;
    }
}