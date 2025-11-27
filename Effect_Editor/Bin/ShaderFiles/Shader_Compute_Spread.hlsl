struct ParticleVertices
{
    float4  vRight;
    float4  vUp;
    float4  vLook;
    float4  vTranslation;
    
    float2 vLifeTime;
    float4x4 WorldMat;
    float4  vfRoot;
    float4  vfStart;
    float2  vfSpeed;
    float   vfSize;
};

cbuffer ParticleBuffer : register(b0)
{
    matrix g_WorldMatrix;
    float4 vfPivot;
    float4 vfGravity;
    float4 vfTimeDelta;
    float2 vfCircle;
    float2 vfTurnPower;
    float2 vfisSphere;
    int2   viLoopAndCount;
}


StructuredBuffer<ParticleVertices> Input : register(t0);
RWStructuredBuffer<ParticleVertices> g_Out : register(u0);

[numthreads(256, 1, 1)]
void CS(uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
           uint GI : SV_GroupIndex)
{
    bool bisStart = false;
    mul(Input[DTid.x].vfRoot, g_WorldMatrix);
    float4 vDir = normalize(mul(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0), Input[DTid.x].WorldMat));
    if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
    {
        vDir.xyzw = 0;
    }
    if (DTid.x >= viLoopAndCount.y)
        return;
    g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
    if (0 > g_Out[DTid.x].vLifeTime.x)
    {
        if (vfTimeDelta.z < abs(g_Out[DTid.x].vLifeTime.x))
            fmod(g_Out[DTid.x].vLifeTime.x, vfTimeDelta.z);
        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
        g_Out[DTid.x].WorldMat = g_WorldMatrix;
        vDir = normalize(mul(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0), g_Out[DTid.x].WorldMat));
        if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
            vDir.xyzw = 0;
        if (0 > g_Out[DTid.x].vLifeTime.x)
            return;
        else if (0 == vfisSphere.x)
        {
            g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot * length(Input[DTid.x].WorldMat._11_12_13);
            g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
        }
        else if(1 == vfisSphere.x)
        {
            g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * vfisSphere.y * length(Input[DTid.x].WorldMat._11_12_13);
            g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
        }
        else if (2 == vfisSphere.x)
        {
            if (1 == viLoopAndCount.x)
            {
                float4 vRight = float4(g_WorldMatrix._11_12_13 * sin(vfTimeDelta.y), 0);
                float4 vUp = float4(g_WorldMatrix._31_32_33 * cos(vfTimeDelta.y), 0);
                g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                vRight = float4(g_WorldMatrix._11_12_13 * sin(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                vUp = float4(g_WorldMatrix._31_32_33 * cos(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                vDir = float4(Input[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0);
            }
            else
            {
                
                float angle = float(DTid.x) / viLoopAndCount.y * -vfCircle.y;
                float4 vRight = float4(g_WorldMatrix._11_12_13 * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                float4 vUp = float4(g_WorldMatrix._31_32_33 * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                vRight = float4(g_WorldMatrix._11_12_13 * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - angle), 0);
                vUp = float4(g_WorldMatrix._31_32_33 * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - angle), 0);
                g_Out[DTid.x].vTranslation = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                
                vDir = normalize(float4(g_Out[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0));
            }
        }
        bisStart = true;
    }
    if (!bisStart && 0 == g_Out[DTid.x].vLifeTime.x)
    {
        g_Out[DTid.x].WorldMat = g_WorldMatrix;
        vDir = normalize(mul(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0), g_Out[DTid.x].WorldMat));
        if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
            vDir.xyzw = 0;
        int i = vfisSphere.x;
        switch (i)
        {
            case 0:{
                    g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * Input[DTid.x].vfRoot * length(Input[DTid.x].WorldMat._11_12_13);
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                    g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                    bisStart = true;
                }
                break;
            case 1:{
                    g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * vfisSphere.y * length(Input[DTid.x].WorldMat._11_12_13);
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                    g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                    bisStart = true;
                }
                break;
            case 2:{
                    if (0 == viLoopAndCount.x)
                    {
                        g_Out[DTid.x].vLifeTime.x = float(DTid.x) / viLoopAndCount.y * -vfCircle.y;
                        float4 vRight = float4(float3(1, 0, 0) * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                        float4 vUp = float4(float3(0, 0, 1) * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                        g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                        vRight = float4(float3(1, 0, 0) * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - g_Out[DTid.x].vLifeTime.x), 0);
                        vUp = float4(float3(0, 0, 1) * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - g_Out[DTid.x].vLifeTime.x), 0);
                        g_Out[DTid.x].vTranslation = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                        return;
                    }
                    else
                    {
                        float4 vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y - vfTimeDelta.x), 0);
                        float4 vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y - vfTimeDelta.x), 0);
                        g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                        vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y), 0);
                        vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y), 0);
                        g_Out[DTid.x].vTranslation = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    }
                }
                break;
        }

    }
    switch (int(vfisSphere.x))
    {
        case 0:
            if (bisStart)
            {
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x * length(Input[DTid.x].WorldMat._11_12_13);
            }
            else
            {
                g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x * length(Input[DTid.x].WorldMat._11_12_13);
            }
            break;
        case 1:
            if (bisStart)
            {
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x * length(Input[DTid.x].WorldMat._11_12_13);
            }
            else
            {
                g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x * length(Input[DTid.x].WorldMat._11_12_13);
            }
            break;
        case 2:
            if (bisStart)
            {
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x;
            }
            else
            {
                if (2 == vfisSphere.x)
                {
                    vDir = normalize(float4(Input[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0));
                }
                g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x;
            }
            break;
    }
    float t = g_Out[DTid.x].vLifeTime.x / g_Out[DTid.x].vLifeTime.y;
    float fGravity = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * vfGravity.x
                        + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(vfGravity.y)) * 100
 + (-2 * pow(t, 3) + 3 * pow(t, 2)) * vfGravity.z
                        + (pow(t, 3) - pow(t, 2)) * tan(radians(vfGravity.w)) * 100;
    
    g_Out[DTid.x].vTranslation += float4(0, 1, 0, 0) * fGravity * vfTimeDelta.x * length(Input[DTid.x].WorldMat._11_12_13);
    if (bisStart && Input[DTid.x].vfSpeed.x != 0)
    {
        g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz) * Input[DTid.x].vfSize, 0);
    }
    else if (0 >= length(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) || Input[DTid.x].vfSpeed.x == 0)
        g_Out[DTid.x].vLook = Input[DTid.x].vLook;
    else
        g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
    
    g_Out[DTid.x].vRight = float4(normalize(cross(g_Out[DTid.x].WorldMat._21_22_23, g_Out[DTid.x].vLook.xyz)) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
    g_Out[DTid.x].vUp = float4(normalize(cross(g_Out[DTid.x].vLook.xyz, g_Out[DTid.x].vRight.xyz)) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
    
    if (!bisStart)
    {
        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
    }
    if (1 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
    {
        
        switch (int(vfisSphere.x))
        {
            case 0:{
                    g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot * length(Input[DTid.x].WorldMat._11_12_13);
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                }
                break;
            case 1:{
                    g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * vfisSphere.y * length(Input[DTid.x].WorldMat._11_12_13);
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                }
                break;
            case 2:{
                
                    float4 vRight = float4(g_WorldMatrix._11_12_13 * sin(vfTimeDelta.y), 0);
                    float4 vUp = float4(g_WorldMatrix._31_32_33 * cos(vfTimeDelta.y), 0);
                    g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    vRight = float4(g_WorldMatrix._11_12_13 * sin(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                    vUp = float4(g_WorldMatrix._31_32_33 * cos(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                }
                break;
        }
        g_Out[DTid.x].WorldMat = g_WorldMatrix;
        g_Out[DTid.x].vLifeTime.x = fmod(g_Out[DTid.x].vLifeTime.x, g_Out[DTid.x].vLifeTime.y);
    }
}

[numthreads(256, 1, 1)]
void CS5(uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
           uint GI : SV_GroupIndex)
{
    bool bisStart = false;
    float4 vDir = normalize(float4(Input[DTid.x].vTranslation.xyz - vfPivot.xyz, 0));
    if (DTid.x >= viLoopAndCount.y)
        return;
    g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
    if (0 > g_Out[DTid.x].vLifeTime.x)
    {
        if (vfTimeDelta.z < abs(g_Out[DTid.x].vLifeTime.x))
            fmod(g_Out[DTid.x].vLifeTime.x, vfTimeDelta.z);
        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
        if (0 > g_Out[DTid.x].vLifeTime.x)
            return;
        bisStart = true;
    }
    if (bisStart)
    {
        g_Out[DTid.x].vfStart = float4(g_WorldMatrix._41_42_43 + Input[DTid.x].vfRoot.xyz, 1);
        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
    }
    else
    {
        g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x;
    }
    float t = g_Out[DTid.x].vLifeTime.x / g_Out[DTid.x].vLifeTime.y;
    float fGravity = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * vfGravity.x
                        + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(vfGravity.y)) * 100
 + (-2 * pow(t, 3) + 3 * pow(t, 2)) * vfGravity.z
                        + (pow(t, 3) - pow(t, 2)) * tan(radians(vfGravity.w)) * 100;
    
    g_Out[DTid.x].vTranslation += float4(normalize(g_WorldMatrix._21_22_23), 0) * fGravity * vfTimeDelta.x;
    if (bisStart)
    {
        g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfRoot.xyz) * Input[DTid.x].vfSize, 0);
    }
    else if (0 >= length(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz))
        g_Out[DTid.x].vLook = Input[DTid.x].vLook;
    else
        g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) * Input[DTid.x].vfSize, 0);
    
    g_Out[DTid.x].vRight = float4(normalize(cross(float3(0.f, 1.f, 0.f), g_Out[DTid.x].vLook.xyz)) * Input[DTid.x].vfSize, 0);
    g_Out[DTid.x].vUp = float4(normalize(cross(g_Out[DTid.x].vLook.xyz, g_Out[DTid.x].vRight.xyz)) * Input[DTid.x].vfSize, 0);
    if (!bisStart)
    {
        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
    }
    if (1 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
    {
        g_Out[DTid.x].vfStart = float4(g_WorldMatrix._41_42_43 + Input[DTid.x].vfRoot.xyz, 1);
        g_Out[DTid.x].vTranslation = float4(g_WorldMatrix._41_42_43 + Input[DTid.x].vfRoot.xyz, 1);
        g_Out[DTid.x].vLifeTime.x = fmod(g_Out[DTid.x].vLifeTime.x, g_Out[DTid.x].vLifeTime.y);
    }
}

[numthreads(256, 1, 1)]
void Tornado(uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
           uint GI : SV_GroupIndex)
{
    bool bisStart = false;
    mul(Input[DTid.x].vfRoot, g_WorldMatrix);
    float4 vDir = normalize(mul(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0), Input[DTid.x].WorldMat));
    if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
    {
        vDir.xyzw = 0;
    }
    if (DTid.x >= viLoopAndCount.y)
        return;
    g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
    if (0 > g_Out[DTid.x].vLifeTime.x)
    {
        if (vfTimeDelta.z < abs(g_Out[DTid.x].vLifeTime.x))
            fmod(g_Out[DTid.x].vLifeTime.x, vfTimeDelta.z);
        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
        g_Out[DTid.x].WorldMat = g_WorldMatrix;
        vDir = normalize(mul(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0), g_Out[DTid.x].WorldMat));
        if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
            vDir.xyzw = 0;
        if (0 > g_Out[DTid.x].vLifeTime.x)
            return;
        else if (0 == vfisSphere.x)
        {
            g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot * length(Input[DTid.x].WorldMat._11_12_13);
            g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
        }
        else if (1 == vfisSphere.x)
        {
            g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * vfisSphere.y * length(Input[DTid.x].WorldMat._11_12_13);
            g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
        }
        else if (2 == vfisSphere.x)
        {
            if (1 == viLoopAndCount.x)
            {
                float4 vRight = float4(g_WorldMatrix._11_12_13 * sin(vfTimeDelta.y), 0);
                float4 vUp = float4(g_WorldMatrix._31_32_33 * cos(vfTimeDelta.y), 0);
                g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                vRight = float4(g_WorldMatrix._11_12_13 * sin(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                vUp = float4(g_WorldMatrix._31_32_33 * cos(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                vDir = float4(Input[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0);
            }
            else
            {
                
                float angle = float(DTid.x) / viLoopAndCount.y * -vfCircle.y;
                float4 vRight = float4(g_WorldMatrix._11_12_13 * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                float4 vUp = float4(g_WorldMatrix._31_32_33 * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                vRight = float4(g_WorldMatrix._11_12_13 * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - angle), 0);
                vUp = float4(g_WorldMatrix._31_32_33 * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - angle), 0);
                g_Out[DTid.x].vTranslation = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                
                vDir = normalize(float4(g_Out[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0));
            }
        }
        bisStart = true;
    }
    
    float4 vRight = normalize(float4(cross(g_WorldMatrix._21_22_23, vDir.xyz), 0));
    float4 vUp = normalize(float4(cross(vDir.xyz, vRight.xyz), 0));
    
    if (!bisStart && 0 == g_Out[DTid.x].vLifeTime.x)
    {
        g_Out[DTid.x].WorldMat = g_WorldMatrix;
        vDir = normalize(mul(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0), g_Out[DTid.x].WorldMat));
        if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
            vDir.xyzw = 0;
        int i = vfisSphere.x;
        switch (i)
        {
            case 0:{
                    g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * Input[DTid.x].vfRoot * length(Input[DTid.x].WorldMat._11_12_13);
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                    g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                    bisStart = true;
                }
                break;
            case 1:{
                    g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * vfisSphere.y;
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                    g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                    bisStart = true;
                }
                break;
            case 2:{
                    if (0 == viLoopAndCount.x)
                    {
                        g_Out[DTid.x].vLifeTime.x = float(DTid.x) / viLoopAndCount.y * -vfCircle.y;
                        float4 vRight = float4(float3(1, 0, 0) * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                        float4 vUp = float4(float3(0, 0, 1) * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                        g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                        vRight = float4(float3(1, 0, 0) * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - g_Out[DTid.x].vLifeTime.x), 0);
                        vUp = float4(float3(0, 0, 1) * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - g_Out[DTid.x].vLifeTime.x), 0);
                        g_Out[DTid.x].vTranslation = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                        return;
                    }
                    else
                    {
                        float4 vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y - vfTimeDelta.x), 0);
                        float4 vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y - vfTimeDelta.x), 0);
                        g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                        vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y), 0);
                        vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y), 0);
                        g_Out[DTid.x].vTranslation = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    }
                }
                break;
        }

    }
    switch (int(vfisSphere.x))
    {
        case 0:
            g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart + (vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x + (vRight * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y))) * length(Input[DTid.x].WorldMat._11_12_13);
            break;
        case 1:
            if (bisStart)
            {
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x;
            }
            else
            {
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart + (vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x + (vRight * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y))) * length(Input[DTid.x].WorldMat._11_12_13);
            }
            break;
        case 2:
            if (bisStart)
            {
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x;
            }
            else
            {
                if (2 == vfisSphere.x)
                {
                    vDir = normalize(float4(Input[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0));
                }
                g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x;
            }
            break;
    }
    float t = g_Out[DTid.x].vLifeTime.x / g_Out[DTid.x].vLifeTime.y;
    float fGravity = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * vfGravity.x
                        + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(vfGravity.y)) * 100
 + (-2 * pow(t, 3) + 3 * pow(t, 2)) * vfGravity.z
                        + (pow(t, 3) - pow(t, 2)) * tan(radians(vfGravity.w)) * 100;
    
    g_Out[DTid.x].vTranslation += float4(0, 1, 0, 0) * fGravity * vfTimeDelta.x * length(Input[DTid.x].WorldMat._11_12_13);
    if (bisStart && Input[DTid.x].vfSpeed.x != 0)
    {
        g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
    }
    else if (0 >= length(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) || Input[DTid.x].vfSpeed.x == 0)
        g_Out[DTid.x].vLook = Input[DTid.x].vLook;
    else
        g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
    
    g_Out[DTid.x].vRight = float4(normalize(cross(g_Out[DTid.x].WorldMat._21_22_23, g_Out[DTid.x].vLook.xyz)) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
    g_Out[DTid.x].vUp = float4(normalize(cross(g_Out[DTid.x].vLook.xyz, g_Out[DTid.x].vRight.xyz)) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
    
    if (!bisStart)
    {
        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
    }
    if (1 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
    {
        
        switch (int(vfisSphere.x))
        {
            case 0:{
                    g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot * length(Input[DTid.x].WorldMat._11_12_13);
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                }
                break;
            case 1:{
                    g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * vfisSphere.y * length(Input[DTid.x].WorldMat._11_12_13);
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                }
                break;
            case 2:{
                
                    float4 vRight = float4(g_WorldMatrix._11_12_13 * sin(vfTimeDelta.y), 0);
                    float4 vUp = float4(g_WorldMatrix._31_32_33 * cos(vfTimeDelta.y), 0);
                    g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    vRight = float4(g_WorldMatrix._11_12_13 * sin(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                    vUp = float4(g_WorldMatrix._31_32_33 * cos(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                }
                break;
        }
        g_Out[DTid.x].WorldMat = g_WorldMatrix;
        g_Out[DTid.x].vLifeTime.x = fmod(g_Out[DTid.x].vLifeTime.x, g_Out[DTid.x].vLifeTime.y);
    }
}

[numthreads(256, 1, 1)]
void Roll(uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
           uint GI : SV_GroupIndex)
{
    bool bisStart = false;
    if (DTid.x >= viLoopAndCount.y)
        return;
    g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
    if (0 > g_Out[DTid.x].vLifeTime.x)
    {
        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
        if (0 > g_Out[DTid.x].vLifeTime.x)
            return;
        else if (1 == vfisSphere.x)
        {
            float4 vDir = normalize(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0));
            float4 vRight = normalize(float4(cross(float3(0.f, 1.f, 0.f), vDir.xyz), 0));
            float4 vUp = normalize(float4(cross(vDir.xyz, vRight.xyz), 0));
            g_Out[DTid.x].vfRoot = vfPivot + vDir * vfisSphere.y;
            g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfRoot + (vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x) + (vRight * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y));
            bisStart = true;
        }
        else if (2 == vfisSphere.x)
        {
            float4 vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y), 0);
            float4 vUp = float4(float3(0, 1, 0) * cos(vfTimeDelta.y), 0);
            g_Out[DTid.x].vfRoot = g_WorldMatrix._41_42_43_44 + vRight * vfisSphere.y + vUp * vfisSphere.y;
            
            float4 vDir = normalize(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0));
            vRight = normalize(float4(cross(float3(0.f, 1.f, 0.f), vDir.xyz), 0));
            vUp = normalize(float4(cross(vDir.xyz, vRight.xyz), 0));
            g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfRoot + (vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x) + (vRight * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y));
            bisStart = true;
        }
    }
    g_Out[DTid.x].vRight = Input[DTid.x].vRight;
    g_Out[DTid.x].vUp = Input[DTid.x].vUp;
    g_Out[DTid.x].vLook = Input[DTid.x].vLook;
    
    float t = g_Out[DTid.x].vLifeTime.x / g_Out[DTid.x].vLifeTime.y;
    float fGravity = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * vfGravity.x
                        + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(vfGravity.y)) * 100
 + (-2 * pow(t, 3) + 3 * pow(t, 2)) * vfGravity.z
                        + (pow(t, 3) - pow(t, 2)) * tan(radians(vfGravity.w)) * 100;
    
    float4 vDir = normalize(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0));
    float4 vRight = normalize(float4(cross(float3(0.f, 1.f, 0.f), vDir.xyz), 0));
    float4 vUp = normalize(float4(cross(vDir.xyz, vRight.xyz), 0));
    
    if (1 == vfisSphere.x && 0 == g_Out[DTid.x].vLifeTime.x)
    {
        g_Out[DTid.x].vfRoot = vfPivot + vDir * vfisSphere.y;
        g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + (vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x) + (vRight * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y));
    }
    if (2 == vfisSphere.x && 0 == g_Out[DTid.x].vLifeTime.x)
    {
        float4 vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y), 0);
        float4 vUp = float4(float3(0, 1, 0) * cos(vfTimeDelta.y), 0);
        g_Out[DTid.x].vfRoot = g_WorldMatrix._41_42_43_44 + vRight * vfisSphere.y + vUp * vfisSphere.y;
            
        float4 vDir = normalize(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0));
        vRight = normalize(float4(cross(float3(0.f, 1.f, 0.f), vDir.xyz), 0));
        vUp = normalize(float4(cross(vDir.xyz, vRight.xyz), 0));
        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfRoot + (vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x) + (vRight * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y));
    }
    else
    {
        if (bisStart)
        {
            g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfRoot + (vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x) + (vRight * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y));
        }
        else
        {
            g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + (vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x) + (vRight * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y));
        }
    }
    g_Out[DTid.x].vTranslation += float4(normalize(g_WorldMatrix._21_22_23), 0) * fGravity * g_Out[DTid.x].vLifeTime.x;
    
    if (!bisStart)
    {
        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
    }
    if (1 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
    {
        if (2 == vfisSphere.x)
        {
            float4 vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y), 0);
            float4 vUp = float4(float3(0, 1, 0) * cos(vfTimeDelta.y), 0);
            g_Out[DTid.x].vfRoot = g_WorldMatrix._41_42_43_44 + vRight * vfisSphere.y + vUp * vfisSphere.y;
            g_Out[DTid.x].vLifeTime.x = -fmod(g_Out[DTid.x].vLifeTime.x, g_Out[DTid.x].vLifeTime.y);
            g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfRoot + (vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x) + (vRight * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y));
        
        }
        else
        {
            g_Out[DTid.x].vLifeTime.x = -fmod(g_Out[DTid.x].vLifeTime.x, g_Out[DTid.x].vLifeTime.y);
            g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + (vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x) + (vRight * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y));
        
        }
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