struct ParticleVertices
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;

    float2 vLifeTime;
    float4 vfRoot;
    float2 vfSpeed;
};

cbuffer ParticleBuffer : register(b0)
{
    matrix g_WorldMatrix;
    float4 vfPivot;
    float4 vfGravity;
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
    if (0 > Input[DTid.x].vLifeTime.x)
    {
        g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
        return;
    }
    g_Out[DTid.x].vRight = Input[DTid.x].vRight;
    g_Out[DTid.x].vUp = Input[DTid.x].vUp;
    g_Out[DTid.x].vLook = Input[DTid.x].vLook;
    float4 vDir = normalize(float4(Input[DTid.x].vTranslation.xyz - vfPivot.xyz, 0));
    float t = Input[DTid.x].vLifeTime.x / Input[DTid.x].vLifeTime.y;
    float fGravity = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * vfGravity.x
                        + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(vfGravity.y)) * 100
 + (-2 * pow(t, 3) + 3 * pow(t, 2)) * vfGravity.z
                        + (pow(t, 3) - pow(t, 2)) * tan(radians(vfGravity.w)) * 100;
    
    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x;
    g_Out[DTid.x].vTranslation += float4(normalize(g_WorldMatrix._21_22_23), 0) * fGravity * vfTimeDelta.x;
    g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
    g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
    
    if (1 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
    {
        g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot;
        g_Out[DTid.x].vLifeTime.x = fmod(g_Out[DTid.x].vLifeTime.x, g_Out[DTid.x].vLifeTime.y);
    }
}


[numthreads(256, 1, 1)]
void CS2(uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
           uint GI : SV_GroupIndex)
{
    if (DTid.x >= viLoopAndCount.y)
        return;
    if (0 > Input[DTid.x].vLifeTime.x)
    {
        g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
        return;
    }
    g_Out[DTid.x].vRight = Input[DTid.x].vRight;
    g_Out[DTid.x].vUp = Input[DTid.x].vUp;
    g_Out[DTid.x].vLook = Input[DTid.x].vLook;
    
    float t = Input[DTid.x].vLifeTime.x / Input[DTid.x].vLifeTime.y;
    float fGravity = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * vfGravity.x
                        + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(vfGravity.y)) * 100
 + (-2 * pow(t, 3) + 3 * pow(t, 2)) * vfGravity.z
                        + (pow(t, 3) - pow(t, 2)) * tan(radians(vfGravity.w)) * 100;
    
    float4 vDir = normalize(float4(Input[DTid.x].vTranslation.xyz - vfPivot.xyz, 0));
    
    float4 vRight = normalize(float4(cross(float3(0.f, 1.f, 0.f), vDir.xyz), 0));
    float4 vUp = normalize(float4(cross(vDir.xyz, vRight.xyz), 0));
    g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
    g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
    g_Out[DTid.x].vTranslation += vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x / 2 + vRight * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x / 2;
    //g_Out[DTid.x].vTranslation += float4(normalize(g_WorldMatrix._21_22_23), 0) * fGravity * vfTimeDelta.x;
    if (1 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
    {
        g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot;
        g_Out[DTid.x].vLifeTime.x = fmod(g_Out[DTid.x].vLifeTime.x, g_Out[DTid.x].vLifeTime.y);
    }
}


[numthreads(256, 1, 1)]
void CS3(uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
           uint GI : SV_GroupIndex)
{
    if (DTid.x >= viLoopAndCount.y)
        return;
    float4 vDir = normalize(float4(Input[DTid.x].vTranslation.xyz - vfPivot.xyz, 0));
    if (0 > Input[DTid.x].vLifeTime.x)
    {
        g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vDir * Input[DTid.x].vfSpeed.x * Input[DTid.x].vLifeTime.y;
        g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
        return;
    }
    g_Out[DTid.x].vRight = Input[DTid.x].vRight;
    g_Out[DTid.x].vUp = Input[DTid.x].vUp;
    g_Out[DTid.x].vLook = Input[DTid.x].vLook;
    float t = Input[DTid.x].vLifeTime.x / Input[DTid.x].vLifeTime.y;
    float fGravity = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * vfGravity.x
                        + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(vfGravity.y)) * 100
 + (-2 * pow(t, 3) + 3 * pow(t, 2)) * vfGravity.z
                        + (pow(t, 3) - pow(t, 2)) * tan(radians(vfGravity.w)) * 100;
    
    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation - vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x;
    g_Out[DTid.x].vTranslation += float4(normalize(g_WorldMatrix._21_22_23), 0) * fGravity * vfTimeDelta.x;
    g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
    g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
    if (1 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
    {
        g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vDir * Input[DTid.x].vfSpeed.x * Input[DTid.x].vLifeTime.y;
        g_Out[DTid.x].vLifeTime.x = fmod(g_Out[DTid.x].vLifeTime.x, g_Out[DTid.x].vLifeTime.y);
    }
}