struct ParticleVertices
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    
    float2 vLifeTime;
    float4x4 WorldMat;
    float4 vfRoot;
    float4 vfStart;
    float2 vfSpeed;
    float vfSize;
};

cbuffer ParticleBuffer : register(b0)
{
    row_major matrix g_WorldMatrix;
    float4 vfPivot;
    float4 vfGravity;
    float4 vfTimeDelta;
    float2 vfCircle;
    float2 vfTurnPower;
    float2 vfisSphere;
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
    bool bisStart = false;
    float2 fSpectrum = float2(0, 0);
    if (vfPivot.w == 0)
    {
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
            if (0 == viLoopAndCount.x)
                return;
            g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
            g_Out[DTid.x].WorldMat = g_WorldMatrix;
            vDir = normalize(mul(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0), g_Out[DTid.x].WorldMat));
            if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
                vDir.xyzw = 0;
            if (0 > g_Out[DTid.x].vLifeTime.x)
                return;
            else if (0 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = mul(g_Out[DTid.x].vfRoot, g_Out[DTid.x].WorldMat);
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
            }
            else if (1 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * vfisSphere.y * length(g_Out[DTid.x].WorldMat._11_12_13);
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
                        g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot * length(g_Out[DTid.x].WorldMat._11_12_13);
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                        bisStart = true;
                    }
                    break;
                case 1:{
                        g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * vfisSphere.y * length(g_Out[DTid.x].WorldMat._11_12_13);
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                        bisStart = true;
                    }
                    break;
                case 2:{
                        if (0 == viLoopAndCount.x)
                        {
                            g_Out[DTid.x].vLifeTime.x = float(DTid.x + 1) / viLoopAndCount.y * -vfCircle.y;
                            return;
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
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x * length(Input[DTid.x].WorldMat._11_12_13);
                }
                else
                {
                    vDir = normalize(float4(Input[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0));
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x * length(Input[DTid.x].WorldMat._11_12_13);
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
            g_Out[DTid.x].vLook = Input[DTid.x].WorldMat._31_32_33_34;
        else
            g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
    
        g_Out[DTid.x].vRight = float4(normalize(cross(g_Out[DTid.x].WorldMat._21_22_23, g_Out[DTid.x].vLook.xyz)) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
        g_Out[DTid.x].vUp = float4(normalize(cross(g_Out[DTid.x].vLook.xyz, g_Out[DTid.x].vRight.xyz)) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
    
        if (!bisStart)
        {
            g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
        }
        if ((0 < viLoopAndCount.x && 5 > viLoopAndCount.x) && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
        
            switch (int(vfisSphere.x))
            {
                case 0:{
                        g_Out[DTid.x].vfStart = mul(g_Out[DTid.x].vfRoot, g_Out[DTid.x].WorldMat);
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
        else if (5 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
            g_Out[DTid.x].vLifeTime.x = vfTimeDelta.w - vfTimeDelta.y;
        }
    }
    else
    {
        float4 vDir = normalize(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0));
        if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
        {
            vDir = 0;
        }
        if (DTid.x >= viLoopAndCount.y)
            return;
        g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
        if (0 > g_Out[DTid.x].vLifeTime.x)
        {
            if (0 == viLoopAndCount.x && 2 != vfisSphere.x)
                return;
            if (4 == viLoopAndCount.x)
                return;
            g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
            vDir = normalize(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0));
            if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
                vDir = 0;
            if (0 > g_Out[DTid.x].vLifeTime.x)
                return;
            else if (0 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot;
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
            }
            else if (1 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = float4((vDir * vfisSphere.y).xyz, 1);
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
            }
            else if (2 == vfisSphere.x)
            {
                if (1 == viLoopAndCount.x)
                {
                    float4 vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y), 0);
                    float4 vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y), 0);
                    g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                    vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    vDir = float4(Input[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0);
                }
                else
                {
                
                    float angle = float(DTid.x + 1) / viLoopAndCount.y * -vfCircle.y;
                    float4 vRight = float4(float3(1, 0, 0) * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                    float4 vUp = float4(float3(0, 0, 1) * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                    g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    vRight = float4(float3(1, 0, 0) * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - angle), 0);
                    vUp = float4(float3(0, 0, 1) * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - angle), 0);
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                
                    vDir = normalize(float4(g_Out[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0));
                }
            }
            bisStart = true;
        }
        if (!bisStart && 0 == g_Out[DTid.x].vLifeTime.x)
        {
            vDir = normalize(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0));
            if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
                vDir.xyzw = 0;
            int i = vfisSphere.x;
            switch (i)
            {
                case 0:{
                        g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot;
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                        bisStart = true;
                    }
                    break;
                case 1:{
                        g_Out[DTid.x].vfStart = float4((vDir * vfisSphere.y).xyz, 1);
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                        bisStart = true;
                    }
                    break;
                case 2:{
                        if (0 == viLoopAndCount.x)
                        {
                            g_Out[DTid.x].vLifeTime.x = float(DTid.x + 1) / viLoopAndCount.y * -vfCircle.y;
                            return;
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
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x;
                }
                else
                {
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x;
                }
                break;
            case 1:
                if (bisStart)
                {
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x;
                }
                else
                {
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x;
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
    
        g_Out[DTid.x].vTranslation.xyz += normalize(g_WorldMatrix._12_22_32) * fGravity * vfTimeDelta.x;
        if (bisStart && Input[DTid.x].vfSpeed.x != 0)
        {
            g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
        }
        else if (0 >= length(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) || Input[DTid.x].vfSpeed.x == 0)
            g_Out[DTid.x].vLook = Input[DTid.x].vLook;
        else
            g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
    
        g_Out[DTid.x].vRight = float4(normalize(cross(float3(0, 1, 0), g_Out[DTid.x].vLook.xyz)) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
        g_Out[DTid.x].vUp = float4(normalize(cross(g_Out[DTid.x].vLook.xyz, g_Out[DTid.x].vRight.xyz)) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
        
        if (!bisStart)
        {
            g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
        }
        if (1 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
        
            switch (int(vfisSphere.x))
            {
                case 0:{
                        g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot;
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                    }
                    break;
                case 1:{
                        g_Out[DTid.x].vfStart = float4((vDir * vfisSphere.y).xyz, 1);
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                    }
                    break;
                case 2:{
                
                        float4 vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y), 0);
                        float4 vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y), 0);
                        g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                        vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                        vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                        g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    }
                    break;
            }
            g_Out[DTid.x].vLifeTime.x = fmod(g_Out[DTid.x].vLifeTime.x, g_Out[DTid.x].vLifeTime.y);
        }
        else if (5 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
            g_Out[DTid.x].vLifeTime.x = vfTimeDelta.w - vfTimeDelta.y;
        }
    }
}


[numthreads(256, 1, 1)]
void Fake_Floor(uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
           uint GI : SV_GroupIndex)
{
    bool bisStart = false;
    float2 fSpectrum = float2(0, 0);
    if (vfPivot.w == 0)
    {
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
            if (0 == viLoopAndCount.x)
                return;
            g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
            g_Out[DTid.x].WorldMat = g_WorldMatrix;
            vDir = normalize(mul(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0), g_Out[DTid.x].WorldMat));
            if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
                vDir.xyzw = 0;
            if (0 > g_Out[DTid.x].vLifeTime.x)
                return;
            else if (0 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = mul(g_Out[DTid.x].vfRoot, g_Out[DTid.x].WorldMat);
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
            }
            else if (1 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * vfisSphere.y * length(g_Out[DTid.x].WorldMat._11_12_13);
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
                        g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + Input[DTid.x].vfRoot * length(g_Out[DTid.x].WorldMat._11_12_13);
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                        bisStart = true;
                    }
                    break;
                case 1:{
                        g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * vfisSphere.y * length(g_Out[DTid.x].WorldMat._11_12_13);
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                        bisStart = true;
                    }
                    break;
                case 2:{
                        if (0 == viLoopAndCount.x)
                        {
                            g_Out[DTid.x].vLifeTime.x = float(DTid.x + 1) / viLoopAndCount.y * -vfCircle.y;
                            return;
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
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x * length(Input[DTid.x].WorldMat._11_12_13);
                }
                else
                {
                    vDir = normalize(float4(Input[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0));
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x * length(Input[DTid.x].WorldMat._11_12_13);
                }
                break;
        }
        float t = g_Out[DTid.x].vLifeTime.x / g_Out[DTid.x].vLifeTime.y;
        float fGravity = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * vfGravity.x
                        + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(vfGravity.y)) * 100
 + (-2 * pow(t, 3) + 3 * pow(t, 2)) * vfGravity.z
                        + (pow(t, 3) - pow(t, 2)) * tan(radians(vfGravity.w)) * 100;
    
        g_Out[DTid.x].vTranslation += float4(0, 1, 0, 0) * fGravity * vfTimeDelta.x * length(Input[DTid.x].WorldMat._11_12_13);
        if (Input[DTid.x].vfStart.y > g_Out[DTid.x].vTranslation.y)
        {
            g_Out[DTid.x].vTranslation.y = Input[DTid.x].vfStart.y;
            g_Out[DTid.x].vTranslation.xz = Input[DTid.x].vTranslation.xz + (g_Out[DTid.x].vTranslation.xz - Input[DTid.x].vTranslation.xz) * saturate((1 - t) * 0.5);
        }
        if (bisStart && Input[DTid.x].vfSpeed.x != 0)
        {
            g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
        }
        else if (0 >= length(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) || Input[DTid.x].vfSpeed.x == 0)
            g_Out[DTid.x].vLook = Input[DTid.x].WorldMat._31_32_33_34;
        else
            g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
    
        g_Out[DTid.x].vRight = float4(normalize(cross(g_Out[DTid.x].WorldMat._21_22_23, g_Out[DTid.x].vLook.xyz)) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
        g_Out[DTid.x].vUp = float4(normalize(cross(g_Out[DTid.x].vLook.xyz, g_Out[DTid.x].vRight.xyz)) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
    
        if (!bisStart)
        {
            g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
        }
        if ((0 < viLoopAndCount.x && 5 > viLoopAndCount.x) && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
        
            switch (int(vfisSphere.x))
            {
                case 0:{
                        g_Out[DTid.x].vfStart = mul(g_Out[DTid.x].vfRoot, g_Out[DTid.x].WorldMat);
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
        else if (5 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
            g_Out[DTid.x].vLifeTime.x = vfTimeDelta.w - vfTimeDelta.y;
        }
    }
    else
    {
        float4 vDir = normalize(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0));
        if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
        {
            vDir = 0;
        }
        if (DTid.x >= viLoopAndCount.y)
            return;
        g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
        if (0 > g_Out[DTid.x].vLifeTime.x)
        {
            if (4 == viLoopAndCount.x)
                return;
            g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
            vDir = normalize(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0));
            if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
                vDir = 0;
            if (0 > g_Out[DTid.x].vLifeTime.x)
                return;
            else if (0 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot;
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
            }
            else if (1 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = float4((vDir * vfisSphere.y).xyz, 1);
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
            }
            else if (2 == vfisSphere.x)
            {
                if (1 == viLoopAndCount.x)
                {
                    float4 vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y), 0);
                    float4 vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y), 0);
                    g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                    vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    vDir = float4(Input[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0);
                }
                else
                {
                
                    float angle = float(DTid.x + 1) / viLoopAndCount.y * -vfCircle.y;
                    float4 vRight = float4(float3(1, 0, 0) * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                    float4 vUp = float4(float3(0, 0, 1) * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                    g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    vRight = float4(float3(1, 0, 0) * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - angle), 0);
                    vUp = float4(float3(0, 0, 1) * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - angle), 0);
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                
                    vDir = normalize(float4(g_Out[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0));
                }
            }
            bisStart = true;
        }
        if (!bisStart && 0 == g_Out[DTid.x].vLifeTime.x)
        {
            vDir = normalize(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0));
            if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
                vDir.xyzw = 0;
            int i = vfisSphere.x;
            switch (i)
            {
                case 0:{
                        g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot;
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                        bisStart = true;
                    }
                    break;
                case 1:{
                        g_Out[DTid.x].vfStart = float4((vDir * vfisSphere.y).xyz, 1);
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                        bisStart = true;
                    }
                    break;
                case 2:{
                        if (0 == viLoopAndCount.x)
                        {
                            g_Out[DTid.x].vLifeTime.x = float(DTid.x + 1) / viLoopAndCount.y * -vfCircle.y;
                            return;
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
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x;
                }
                else
                {
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x;
                }
                break;
            case 1:
                if (bisStart)
                {
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x;
                }
                else
                {
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x;
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
    
        g_Out[DTid.x].vTranslation.xyz += normalize(g_WorldMatrix._12_22_32) * fGravity * vfTimeDelta.x;
        if (bisStart && Input[DTid.x].vfSpeed.x != 0)
        {
            g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
        }
        else if (0 >= length(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) || Input[DTid.x].vfSpeed.x == 0)
            g_Out[DTid.x].vLook = Input[DTid.x].vLook;
        else
            g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
    
        g_Out[DTid.x].vRight = float4(normalize(cross(float3(0, 1, 0), g_Out[DTid.x].vLook.xyz)) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
        g_Out[DTid.x].vUp = float4(normalize(cross(g_Out[DTid.x].vLook.xyz, g_Out[DTid.x].vRight.xyz)) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
        
        if (!bisStart)
        {
            g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
        }
        if (1 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
        
            switch (int(vfisSphere.x))
            {
                case 0:{
                        g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot;
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                    }
                    break;
                case 1:{
                        g_Out[DTid.x].vfStart = float4((vDir * vfisSphere.y).xyz, 1);
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                    }
                    break;
                case 2:{
                
                        float4 vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y), 0);
                        float4 vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y), 0);
                        g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                        vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                        vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                        g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    }
                    break;
            }
            g_Out[DTid.x].vLifeTime.x = fmod(g_Out[DTid.x].vLifeTime.x, g_Out[DTid.x].vLifeTime.y);
        }
        else if (5 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
            g_Out[DTid.x].vLifeTime.x = vfTimeDelta.w - vfTimeDelta.y;
        }
    }
}

float3 CatmullRom(float3 p0, float3 p1, float3 p2, float3 p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * ((2.0f * p1) +
        (-p0 + p2) * t +
        (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
        (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

[numthreads(256, 1, 1)]
void Spectrum(uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
           uint GI : SV_GroupIndex)
{
    float2 fSpectrum = float2(0, 0);
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
        if (5 == viLoopAndCount.x || vfTimeDelta.z == vfTimeDelta.w)
            return;
        fSpectrum.x = vfTimeDelta.z;
        fSpectrum.y = vfTimeDelta.w;
        if (viLoopAndCount.y <= vfTimeDelta.w)
        {
            if (DTid.x <= fmod(vfTimeDelta.w, viLoopAndCount.y))
            {
                fSpectrum.x = 0;
                fSpectrum.y = fmod(vfTimeDelta.w, viLoopAndCount.y);
            }
            else
            {
                fSpectrum.y = viLoopAndCount.y - 1;
            }

        }
        if (4 > viLoopAndCount.x)
        {
            if (DTid.x == int(fSpectrum.y))
            {
                g_Out[DTid.x].WorldMat = g_WorldMatrix;
                g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44;
            }
        }
        else if ((DTid.x > fSpectrum.x && DTid.x <= fSpectrum.y))
        {
            g_Out[DTid.x].vLifeTime.x = vfTimeDelta.x;
            g_Out[DTid.x].WorldMat = g_WorldMatrix;
            float3 fWorld = g_WorldMatrix._41_42_43;
            float fVal = DTid.x - (vfTimeDelta.z + 1);
            if (viLoopAndCount.y <= vfTimeDelta.w)
            {
                fVal = (viLoopAndCount.y - 1) + DTid.x - (vfTimeDelta.z + 1);
            }
            int idx = vfTimeDelta.z - 1;
            if (0 == vfTimeDelta.z)
                idx = viLoopAndCount.y - 1;
            float3 fStart1 = Input[idx].vfStart.xyz;
            float3 fStart2 = Input[int(vfTimeDelta.z)].vfStart.xyz;
            if (1 != vfTimeDelta.w - vfTimeDelta.z)
            {
                if (0 == fVal)
                {
                    g_Out[DTid.x].vfStart = float4(CatmullRom(fStart1, fStart2, fWorld, fWorld + (fWorld - fStart2), fVal / (vfTimeDelta.w - vfTimeDelta.z - 1)), 1);
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                    g_Out[DTid.x].WorldMat._31_32_33_34 = float4(normalize(g_Out[DTid.x].vfStart.xyz - fStart2) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
                    g_Out[DTid.x].WorldMat._11_12_13_24 = float4(normalize(cross(g_Out[DTid.x].WorldMat._21_22_23, g_Out[DTid.x].WorldMat._31_32_33)) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
                    g_Out[DTid.x].WorldMat._21_22_23_24 = float4(normalize(cross(g_Out[DTid.x].WorldMat._31_32_33, g_Out[DTid.x].WorldMat._11_12_13)) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
                }
                else
                {
                    g_Out[DTid.x].vfStart = float4(CatmullRom(fStart1, fStart2, fWorld, fWorld + (fWorld - fStart2), fVal / (vfTimeDelta.w - vfTimeDelta.z - 1)), 1);
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                    g_Out[DTid.x].WorldMat._31_32_33_34 = float4(normalize(g_Out[DTid.x].vfStart.xyz - CatmullRom(fStart1, fStart2, fWorld, fWorld + (fWorld - fStart2), (fVal - 1) / (vfTimeDelta.w - vfTimeDelta.z - 1))) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
                    g_Out[DTid.x].WorldMat._11_12_13_24 = float4(normalize(cross(g_Out[DTid.x].WorldMat._21_22_23, g_Out[DTid.x].WorldMat._31_32_33)) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
                    g_Out[DTid.x].WorldMat._21_22_23_24 = float4(normalize(cross(g_Out[DTid.x].WorldMat._31_32_33, g_Out[DTid.x].WorldMat._11_12_13)) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
                }
            }
            else
            {
                g_Out[DTid.x].vfStart = float4(fWorld, 1);
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                g_Out[DTid.x].WorldMat._31_32_33_34 = float4(normalize(g_Out[DTid.x].vfStart.xyz - fStart2) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
                g_Out[DTid.x].WorldMat._11_12_13_24 = float4(normalize(cross(g_Out[DTid.x].WorldMat._21_22_23, g_Out[DTid.x].WorldMat._31_32_33)) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
                g_Out[DTid.x].WorldMat._21_22_23_24 = float4(normalize(cross(g_Out[DTid.x].WorldMat._31_32_33, g_Out[DTid.x].WorldMat._11_12_13)) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
                
            }
        }
    }
    else
    {
        switch (int(vfisSphere.x))
        {
            case 0:
                g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x * length(Input[DTid.x].WorldMat._11_12_13);
                break;
            case 1:
                g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x * length(Input[DTid.x].WorldMat._11_12_13);
                break;
            case 2:
                vDir = normalize(float4(Input[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0));
                g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation; // + vDir * Input[DTid.x].vfSpeed.x * vfTimeDelta.x * length(Input[DTid.x].WorldMat._11_12_13);
                break;
        }
        float t = g_Out[DTid.x].vLifeTime.x / g_Out[DTid.x].vLifeTime.y;
        float fGravity = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * vfGravity.x
                        + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(vfGravity.y)) * 100
 + (-2 * pow(t, 3) + 3 * pow(t, 2)) * vfGravity.z
                        + (pow(t, 3) - pow(t, 2)) * tan(radians(vfGravity.w)) * 100;
    
        g_Out[DTid.x].vTranslation += float4(0, 1, 0, 0) * fGravity * vfTimeDelta.x * length(Input[DTid.x].WorldMat._11_12_13);
        if (0 >= length(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) || Input[DTid.x].vfSpeed.x == 0)
            g_Out[DTid.x].vLook = Input[DTid.x].vLook;
        else
            g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
    
        g_Out[DTid.x].vRight = float4(normalize(cross(g_Out[DTid.x].WorldMat._21_22_23, g_Out[DTid.x].vLook.xyz)) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
        g_Out[DTid.x].vUp = float4(normalize(cross(g_Out[DTid.x].vLook.xyz, g_Out[DTid.x].vRight.xyz)) * Input[DTid.x].vfSize * length(Input[DTid.x].WorldMat._11_12_13), 0);
    
        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
        if ((2 == viLoopAndCount.x || 3 == viLoopAndCount.x || 4 == viLoopAndCount.x) && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
            g_Out[DTid.x].vLifeTime.x = -1;
        }
    }
}


[numthreads(256, 1, 1)]
void Tornado(uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
           uint GI : SV_GroupIndex)
{
    bool bisStart = false;
    if (vfPivot.w == 0)
    {
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
            g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
            g_Out[DTid.x].WorldMat = g_WorldMatrix;
            vDir = normalize(mul(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0), g_Out[DTid.x].WorldMat));
            if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
                vDir.xyzw = 0;
            if (0 > g_Out[DTid.x].vLifeTime.x)
                return;
            else if (0 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = mul(g_Out[DTid.x].vfRoot, g_Out[DTid.x].WorldMat);
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
            }
            else if (1 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * vfisSphere.y * length(g_Out[DTid.x].WorldMat._11_12_13);
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
                        g_Out[DTid.x].vfStart = mul(g_Out[DTid.x].vfRoot, g_Out[DTid.x].WorldMat);
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                        bisStart = true;
                    }
                    break;
                case 1:{
                        g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * vfisSphere.y * length(g_Out[DTid.x].WorldMat._11_12_13);
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
        if ((1 == viLoopAndCount.x || 2 == viLoopAndCount.x || 3 == viLoopAndCount.x || 4 == viLoopAndCount.x) && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
        
            switch (int(vfisSphere.x))
            {
                case 0:{
                        g_Out[DTid.x].vfStart = mul(g_Out[DTid.x].vfRoot, g_Out[DTid.x].WorldMat);
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
        else if (2 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
            g_Out[DTid.x].vLifeTime.x = vfTimeDelta.w - vfTimeDelta.y;
        }
    }
    else
    {
        float4 vDir = normalize(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0));
        if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
        {
            vDir = 0;
        }
        if (DTid.x >= viLoopAndCount.y)
            return;
        g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
        if (0 > g_Out[DTid.x].vLifeTime.x)
        {
            if (0 == viLoopAndCount.x && 2 != vfisSphere.x)
                return;
            if (2 == viLoopAndCount.x)
                return;
            g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
            vDir = normalize(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0));
            if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
                vDir = 0;
            if (0 > g_Out[DTid.x].vLifeTime.x)
                return;
            else if (0 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot;
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
            }
            else if (1 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = float4((vDir * vfisSphere.y).xyz, 1);
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
            }
            else if (2 == vfisSphere.x)
            {
                if (1 == viLoopAndCount.x)
                {
                    float4 vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y), 0);
                    float4 vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y), 0);
                    g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                    vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    vDir = float4(Input[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0);
                }
                else
                {
                
                    float angle = float(DTid.x + 1) / viLoopAndCount.y * -vfCircle.y;
                    float4 vRight = float4(float3(1, 0, 0) * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                    float4 vUp = float4(float3(0, 0, 1) * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                    g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    vRight = float4(float3(1, 0, 0) * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - angle), 0);
                    vUp = float4(float3(0, 0, 1) * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - angle), 0);
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                
                    vDir = normalize(float4(g_Out[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0));
                }
            }
            bisStart = true;
        }
        
    
        float4 vRight = normalize(float4(cross(float3(0, 1, 0), vDir.xyz), 0));
        float4 vUp = normalize(float4(cross(vDir.xyz, vRight.xyz), 0));
        
        if (!bisStart && 0 == g_Out[DTid.x].vLifeTime.x)
        {
            vDir = normalize(float4(Input[DTid.x].vfRoot.xyz - vfPivot.xyz, 0));
            if (0 >= length(Input[DTid.x].vfRoot.xyz - vfPivot.xyz))
                vDir.xyzw = 0;
            int i = vfisSphere.x;
            switch (i)
            {
                case 0:{
                        g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot;
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                        bisStart = true;
                    }
                    break;
                case 1:{
                        g_Out[DTid.x].vfStart = float4((vDir * vfisSphere.y).xyz, 1);
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                        bisStart = true;
                    }
                    break;
                case 2:{
                        if (0 == viLoopAndCount.x)
                        {
                            g_Out[DTid.x].vLifeTime.x = float(DTid.x + 1) / viLoopAndCount.y * -vfCircle.y;
                            return;
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
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + (vDir * Input[DTid.x].vfSpeed.x + (vRight * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y))) * g_Out[DTid.x].vLifeTime.x;
                }
                else
                {
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + (vDir * Input[DTid.x].vfSpeed.x + (vRight * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y))) * vfTimeDelta.x;
                }
                break;
            case 1:
                if (bisStart)
                {
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + (vDir * Input[DTid.x].vfSpeed.x + (vRight * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y))) * g_Out[DTid.x].vLifeTime.x;
                }
                else
                {
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + (vDir * Input[DTid.x].vfSpeed.x + (vRight * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y))) * vfTimeDelta.x;
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
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + (vDir * Input[DTid.x].vfSpeed.x + (vRight * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y))) * vfTimeDelta.x;
                }
                break;
        }
        float t = g_Out[DTid.x].vLifeTime.x / g_Out[DTid.x].vLifeTime.y;
        float fGravity = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * vfGravity.x
                        + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(vfGravity.y)) * 100
 + (-2 * pow(t, 3) + 3 * pow(t, 2)) * vfGravity.z
                        + (pow(t, 3) - pow(t, 2)) * tan(radians(vfGravity.w)) * 100;
    
        g_Out[DTid.x].vTranslation += normalize(g_WorldMatrix._21_22_23_24) * fGravity * vfTimeDelta.x;
        if (bisStart && Input[DTid.x].vfSpeed.x != 0)
        {
            g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
        }
        else if (0 >= length(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) || Input[DTid.x].vfSpeed.x == 0)
            g_Out[DTid.x].vLook = Input[DTid.x].vLook;
        else
            g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
    
        g_Out[DTid.x].vRight = float4(normalize(cross(float3(0, 1, 0), g_Out[DTid.x].vLook.xyz)) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
        g_Out[DTid.x].vUp = float4(normalize(cross(g_Out[DTid.x].vLook.xyz, g_Out[DTid.x].vRight.xyz)) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
        
        if (!bisStart)
        {
            g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
        }
        if (1 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
        
            switch (int(vfisSphere.x))
            {
                case 0:{
                        g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot;
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                    }
                    break;
                case 1:{
                        g_Out[DTid.x].vfStart = float4((vDir * vfisSphere.y).xyz, 1);
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                    }
                    break;
                case 2:{
                
                        float4 vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y), 0);
                        float4 vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y), 0);
                        g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                        vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                        vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                        g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    }
                    break;
            }
            g_Out[DTid.x].vLifeTime.x = fmod(g_Out[DTid.x].vLifeTime.x, g_Out[DTid.x].vLifeTime.y);
        }
        else if (2 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
            g_Out[DTid.x].vLifeTime.x = vfTimeDelta.w - vfTimeDelta.y;
        }
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
        if (2 == viLoopAndCount.x)
            return;
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
void Converge(uint3 Gid : SV_GroupID,
        uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
           uint GI : SV_GroupIndex)
{
    bool bisStart = false;
    if (vfPivot.w == 0)
    {
        float t = g_Out[DTid.x].vLifeTime.x / g_Out[DTid.x].vLifeTime.y;
        float fGravity = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * vfGravity.x
                        + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(vfGravity.y)) * 100
 + (-2 * pow(t, 3) + 3 * pow(t, 2)) * vfGravity.z
                        + (pow(t, 3) - pow(t, 2)) * tan(radians(vfGravity.w)) * 100;
        
        float4 vDir = normalize(float4(g_WorldMatrix._41_42_43 - Input[DTid.x].vTranslation.xyz, 0));
        if (0 >= length(g_WorldMatrix._41_42_43 - Input[DTid.x].vTranslation.xyz))
        {
            vDir.xyzw = 0;
        }
        if (0 > fGravity)
        {
            vDir = normalize(float4(Input[DTid.x].WorldMat._41_42_43 - Input[DTid.x].vTranslation.xyz, 0));
            if (0 >= length(Input[DTid.x].WorldMat._41_42_43 - Input[DTid.x].vTranslation.xyz))
            {
                vDir.xyzw = 0;
            }
        }
        if (DTid.x >= viLoopAndCount.y)
            return;
        g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
        if (0 > g_Out[DTid.x].vLifeTime.x)
        {
            g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
            g_Out[DTid.x].WorldMat = g_WorldMatrix;
            vDir = normalize(mul(float4(g_WorldMatrix._41_42_43 - Input[DTid.x].vTranslation.xyz, 0), g_Out[DTid.x].WorldMat));
            if (0 >= length(g_WorldMatrix._41_42_43 - Input[DTid.x].vTranslation.xyz))
                vDir.xyzw = 0;
            if (0 > fGravity)
            {
                vDir = normalize(float4(Input[DTid.x].WorldMat._41_42_43 - Input[DTid.x].vTranslation.xyz, 0));
                if (0 >= length(Input[DTid.x].WorldMat._41_42_43 - Input[DTid.x].vTranslation.xyz))
                {
                    vDir.xyzw = 0;
                }
            }
            if (0 > g_Out[DTid.x].vLifeTime.x)
                return;
            else if (0 == vfisSphere.x)
            {
                        g_Out[DTid.x].vfStart = mul(g_Out[DTid.x].vfRoot, g_Out[DTid.x].WorldMat);
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
            }
            else if (1 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * vfisSphere.y * length(g_Out[DTid.x].WorldMat._11_12_13);
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
        if (0 > fGravity)
        {
            vRight = normalize(float4(cross(Input[DTid.x].WorldMat._21_22_23, vDir.xyz), 0));
        }
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
                        g_Out[DTid.x].vfStart = mul(g_Out[DTid.x].vfRoot, g_Out[DTid.x].WorldMat);
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                        bisStart = true;
                    }
                    break;
                case 1:{
                        g_Out[DTid.x].vfStart = g_WorldMatrix._41_42_43_44 + vDir * vfisSphere.y * length(g_Out[DTid.x].WorldMat._11_12_13);
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
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x * fGravity;
                }
                else
                {
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + (vDir * Input[DTid.x].vfSpeed.x * fGravity + (vRight * fGravity * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * fGravity * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y))) * vfTimeDelta.x * length(Input[DTid.x].WorldMat._11_12_13) * (length(g_WorldMatrix._41_42_43 - Input[DTid.x].vTranslation.xyz) / length(Input[DTid.x].WorldMat._11_12_13));
                }
                break;
            case 1:
                if (bisStart)
                {
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x * fGravity;
                }
                else
                {
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + (vDir * Input[DTid.x].vfSpeed.x * fGravity + (vRight * fGravity * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * fGravity * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y))) * vfTimeDelta.x * length(Input[DTid.x].WorldMat._11_12_13) * (length(g_WorldMatrix._41_42_43 - Input[DTid.x].vTranslation.xyz) / length(Input[DTid.x].WorldMat._11_12_13)) * 5;
                }
                break;
            case 2:
                if (bisStart)
                {
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * g_Out[DTid.x].vLifeTime.x * fGravity;
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
    }
    else
    {
        float t = g_Out[DTid.x].vLifeTime.x / g_Out[DTid.x].vLifeTime.y;
        float fGravity = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * vfGravity.x
                        + (pow(t, 3) - 2 * pow(t, 2) + t) * tan(radians(vfGravity.y)) * 100
 + (-2 * pow(t, 3) + 3 * pow(t, 2)) * vfGravity.z
                        + (pow(t, 3) - pow(t, 2)) * tan(radians(vfGravity.w)) * 100;
        
        float4 vDir = normalize(float4(vfPivot.xyz - Input[DTid.x].vTranslation.xyz, 0));
        if (0 >= length((vfPivot.xyz - Input[DTid.x].vTranslation.xyz)))
        {
            vDir = 0;
        }
        if (DTid.x >= viLoopAndCount.y)
            return;
        g_Out[DTid.x].vLifeTime = Input[DTid.x].vLifeTime;
        if (0 > g_Out[DTid.x].vLifeTime.x)
        {
            if (2 == viLoopAndCount.x)
                return;
            g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
            vDir = normalize(float4(vfPivot.xyz - Input[DTid.x].vTranslation.xyz, 0));
            if (0 >= length((vfPivot.xyz - Input[DTid.x].vTranslation.xyz)))
                vDir = 0;
            if (0 > g_Out[DTid.x].vLifeTime.x)
                return;
            else if (0 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot;
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
            }
            else if (1 == vfisSphere.x)
            {
                g_Out[DTid.x].vfStart = float4((vDir * vfisSphere.y).xyz, 1);
                g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
            }
            else if (2 == vfisSphere.x)
            {
                if (1 == viLoopAndCount.x)
                {
                    float4 vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y), 0);
                    float4 vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y), 0);
                    g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                    vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    vDir = float4(Input[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0);
                }
                else
                {
                
                    float angle = float(DTid.x + 1) / viLoopAndCount.y * -vfCircle.y;
                    float4 vRight = float4(float3(1, 0, 0) * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                    float4 vUp = float4(float3(0, 0, 1) * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x)), 0);
                    g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    vRight = float4(float3(1, 0, 0) * sin(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - angle), 0);
                    vUp = float4(float3(0, 0, 1) * cos(float(DTid.x) / viLoopAndCount.y * radians(vfCircle.x) - angle), 0);
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                
                    vDir = normalize(float4(g_Out[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0));
                }
            }
            bisStart = true;
        }
        
    
        float4 vRight = normalize(float4(cross(float3(0, 1, 0), vDir.xyz), 0));
        float4 vUp = normalize(float4(cross(vDir.xyz, vRight.xyz), 0));
        
        if (!bisStart && 0 == g_Out[DTid.x].vLifeTime.x)
        {
            vDir = normalize(float4(vfPivot.xyz - Input[DTid.x].vTranslation.xyz, 0));
            if (0 >= length((vfPivot.xyz - Input[DTid.x].vTranslation.xyz)))
                vDir.xyzw = 0;
            int i = vfisSphere.x;
            switch (i)
            {
                case 0:{
                        g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot;
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                        bisStart = true;
                    }
                    break;
                case 1:{
                        g_Out[DTid.x].vfStart = float4((vDir * vfisSphere.y).xyz, 1);
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                        g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
                        bisStart = true;
                    }
                    break;
                case 2:{
                        if (0 == viLoopAndCount.x)
                        {
                            g_Out[DTid.x].vLifeTime.x = float(DTid.x + 1) / viLoopAndCount.y * -vfCircle.y;
                            return;
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
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + (vDir * Input[DTid.x].vfSpeed.x * fGravity + (vRight * fGravity * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * fGravity * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y))) * g_Out[DTid.x].vLifeTime.x;
                }
                else
                {
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + (vDir * Input[DTid.x].vfSpeed.x * fGravity + (vRight * fGravity * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * fGravity * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y))) * vfTimeDelta.x;
                }
                break;
            case 1:
                if (bisStart)
                {
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + (vDir * Input[DTid.x].vfSpeed.x * fGravity + (vRight * fGravity * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * fGravity * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y))) * g_Out[DTid.x].vLifeTime.x;
                }
                else
                {
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + (vDir * Input[DTid.x].vfSpeed.x * fGravity + (vRight * fGravity * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * cos(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y)) + (vUp * fGravity * g_Out[DTid.x].vLifeTime.x * vfTurnPower.x * sin(g_Out[DTid.x].vLifeTime.x * vfTurnPower.y))) * vfTimeDelta.x;
                }
                break;
            case 2:
                if (bisStart)
                {
                    g_Out[DTid.x].vTranslation = g_Out[DTid.x].vTranslation + vDir * Input[DTid.x].vfSpeed.x * fGravity * g_Out[DTid.x].vLifeTime.x;
                }
                else
                {
                    if (2 == vfisSphere.x)
                    {
                        vDir = normalize(float4(Input[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz, 0));
                    }
                    g_Out[DTid.x].vTranslation = Input[DTid.x].vTranslation + vDir * fGravity * Input[DTid.x].vfSpeed.x * vfTimeDelta.x;
                }
                break;
        }
        
        if (bisStart && Input[DTid.x].vfSpeed.x != 0)
        {
            g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - g_Out[DTid.x].vfStart.xyz) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
        }
        else if (0 >= length(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) || Input[DTid.x].vfSpeed.x == 0)
            g_Out[DTid.x].vLook = Input[DTid.x].vLook;
        else
            g_Out[DTid.x].vLook = float4(normalize(g_Out[DTid.x].vTranslation.xyz - Input[DTid.x].vTranslation.xyz) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
    
        g_Out[DTid.x].vRight = float4(normalize(cross(float3(0, 1, 0), g_Out[DTid.x].vLook.xyz)) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
        g_Out[DTid.x].vUp = float4(normalize(cross(g_Out[DTid.x].vLook.xyz, g_Out[DTid.x].vRight.xyz)) * Input[DTid.x].vfSize * length(g_WorldMatrix._11_12_13), 0);
        
        if (!bisStart)
        {
            g_Out[DTid.x].vLifeTime.x += vfTimeDelta.x;
        }
        if (1 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
        
            switch (int(vfisSphere.x))
            {
                case 0:{
                        g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot;
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                    }
                    break;
                case 1:{
                        g_Out[DTid.x].vfStart = float4((vDir * vfisSphere.y).xyz, 1);
                        g_Out[DTid.x].vTranslation = g_Out[DTid.x].vfStart;
                    }
                    break;
                case 2:{
                
                        float4 vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y), 0);
                        float4 vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y), 0);
                        g_Out[DTid.x].vfStart = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                        vRight = float4(float3(1, 0, 0) * sin(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                        vUp = float4(float3(0, 0, 1) * cos(vfTimeDelta.y - g_Out[DTid.x].vLifeTime.x), 0);
                        g_Out[DTid.x].vTranslation = Input[DTid.x].vfRoot + vRight * vfisSphere.y + vUp * vfisSphere.y;
                    }
                    break;
            }
            g_Out[DTid.x].vLifeTime.x = fmod(g_Out[DTid.x].vLifeTime.x, g_Out[DTid.x].vLifeTime.y);
        }
        else if (2 == viLoopAndCount.x && g_Out[DTid.x].vLifeTime.x >= g_Out[DTid.x].vLifeTime.y)
        {
            g_Out[DTid.x].vLifeTime.x = vfTimeDelta.w - vfTimeDelta.y;
        }
    }
}