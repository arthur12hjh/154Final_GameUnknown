float4x4 MakeScaleMatrix(float4 vScale)
{
    return float4x4(
        vScale.x, 0.f, 0.f, 0.f,
        0.f, vScale.y, 0.f, 0.f,
        0.f, 0.f, vScale.z, 0.f,
        0.f, 0.f, 0.f, 1.f
    );
}

float4x4 MakeTranslationMatrix(float4 vTranslation)
{
    return float4x4(
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        vTranslation.x, vTranslation.y, vTranslation.z, 1.f
    );
}

float4x4 MakeRotationMatrix(float4 vRotation)
{
    float x = vRotation.x;
    float y = vRotation.y;
    float z = vRotation.z;
    float w = vRotation.w;

    float xx = x * x;
    float xy = x * y;
    float xz = x * z;
    float xw = x * w;
    float yy = y * y;
    float yz = y * z;
    float yw = y * w;
    float zz = z * z;
    float zw = z * w;

    return float4x4(
        1.f - 2.f * (yy + zz), 2.f * (xy + zw), 2.f * (xz - yw), 0.f,
        2.f * (xy - zw), 1.f - 2.f * (xx + zz), 2.f * (yz + xw), 0.f,
        2.f * (xz + yw), 2.f * (yz - xw), 1.f - 2.f * (xx + yy), 0.f,
        0.f, 0.f, 0.f, 1.f
    );
}

float4 QuaternionSlerp(float4 q1, float4 q2, float t)
{
    float cosTheta = dot(q1, q2);

    if (cosTheta < 0.0f)
    {
        q2 = -q2;
        cosTheta = -cosTheta;
    }

    if (cosTheta > 0.9995f)
        return normalize(lerp(q1, q2, t));

    float theta = acos(cosTheta);
    float sinTheta = sin(theta);

    float w1 = sin((1.f - t) * theta) / sinTheta;
    float w2 = sin(t * theta) / sinTheta;

    return normalize(q1 * w1 + q2 * w2);
}

struct BoneInfo
{
    int iParentIndex;
    int3 _padding;
};

struct ChannelInfo
{
    uint iBoneIndex;
    uint iCurrentKeyFrameIndex;
    uint iNumKeyFrames;
    uint iKeyFrameOffset;
};

struct KeyFrameInfo
{
    float3 vScale;
    float padding01;
    float4 vRotation;
    float3 vTranslation;
    float fTrackPosition;
};

struct BoneTransformMatrixOut
{
    row_major float4x4 BoneLocalTransformMatrix;
    row_major float4x4 BoneCombinedTransformMatrix;
};

cbuffer AnimationGlobalBuffer : register(b0)
{
    row_major float4x4 g_PreTransformMatrix;
    float g_fCurrentTrackPosition;
    float g_fTimeDelta;
    float g_fTickPerSecond;
    float g_fDuration;

    uint g_bIsLoop;
    uint g_iNumBones;
    uint g_iNumChannels;
    uint g_iRootIndex;

    float g_fBlendRatio;
    float3 _padding;
}

StructuredBuffer<BoneInfo> InputBone : register(t0);
StructuredBuffer<ChannelInfo> InputChannel : register(t1);
StructuredBuffer<KeyFrameInfo> InputKeyFrame : register(t2);
StructuredBuffer<BoneTransformMatrixOut> InputLocalMatrix : register(t3);
StructuredBuffer<BoneTransformMatrixOut> PrevLocalMatrix : register(t4);

RWStructuredBuffer<BoneTransformMatrixOut> g_CombinedOut : register(u0);
RWStructuredBuffer<BoneTransformMatrixOut> g_RootOut : register(u1);


uint ComputeKeyFrameIndex(uint boneIndex, float t)
{
    ChannelInfo ch = InputChannel[boneIndex];
    if (ch.iNumKeyFrames <= 1)
        return 0;

    uint start = ch.iKeyFrameOffset;
    uint end = start + ch.iNumKeyFrames - 1;

    if (t >= InputKeyFrame[end].fTrackPosition)
        return ch.iNumKeyFrames - 1;

    for (uint i = 0; i < ch.iNumKeyFrames - 1; ++i)
    {
        float t0 = InputKeyFrame[start + i].fTrackPosition;
        float t1 = InputKeyFrame[start + i + 1].fTrackPosition;

        if (t >= t0 && t < t1)
            return i;
    }

    return ch.iNumKeyFrames - 1;
}

float4x4 ComputeLocalMatrix(uint boneIndex, float t)
{
    ChannelInfo ch = InputChannel[boneIndex];

    if (ch.iNumKeyFrames <= 1)
        return InputLocalMatrix[boneIndex].BoneLocalTransformMatrix;

    uint start = ch.iKeyFrameOffset;
    uint idx0 = start + ComputeKeyFrameIndex(boneIndex, t);
    uint idx1 = min(idx0 + 1, start + ch.iNumKeyFrames - 1);

    float t0 = InputKeyFrame[idx0].fTrackPosition;
    float t1 = InputKeyFrame[idx1].fTrackPosition;
    float dt = max(t1 - t0, 0.0001f);

    float ratio = saturate((t - t0) / dt);

    float3 s0 = InputKeyFrame[idx0].vScale;
    float3 s1 = InputKeyFrame[idx1].vScale;

    float4 r0 = InputKeyFrame[idx0].vRotation;
    float4 r1 = InputKeyFrame[idx1].vRotation;

    float3 p0 = InputKeyFrame[idx0].vTranslation;
    float3 p1 = InputKeyFrame[idx1].vTranslation;

    float3 S = lerp(s0, s1, ratio);
    float4 R = QuaternionSlerp(r0, r1, ratio);
    float3 T = lerp(p0, p1, ratio);

    float4x4 mS = MakeScaleMatrix(float4(S, 1.f));
    float4x4 mR = MakeRotationMatrix(R);
    float4x4 mT = MakeTranslationMatrix(float4(T, 1.f));

    return mul(mul(mS, mR), mT);
}


[numthreads(128, 1, 1)]
void CombinedMatrices(uint3 gid : SV_GroupID,
                      uint3 dtid : SV_DispatchThreadID,
                      uint3 gtid : SV_GroupThreadID,
                      uint gi : SV_GroupIndex)
{
    uint bone = dtid.x;
    if (bone >= g_iNumBones)
        return;

    float t = g_fCurrentTrackPosition;
    if (g_bIsLoop != 0 && g_fDuration > 0.f)
    {
        t = fmod(t, g_fDuration);
        if (t < 0.f)
            t += g_fDuration;
    }

    float4x4 localOriginal = ComputeLocalMatrix(bone, t);

    float4x4 localSkin = localOriginal;
    if (bone == g_iRootIndex)
    {
        localSkin._41 = 0.f;
        localSkin._42 = 0.f;
        localSkin._43 = 0.f;
    }

    float4x4 combinedOriginal = localOriginal;
    float4x4 combinedSkin = localSkin;

    int parent = InputBone[bone].iParentIndex;
    while (parent >= 0)
    {
        float4x4 parentLocal = ComputeLocalMatrix(parent, t);
        combinedOriginal = mul(combinedOriginal, parentLocal);

        float4x4 parentSkin = parentLocal;
        if (parent == g_iRootIndex)
        {
            parentSkin._41 = 0.f;
            parentSkin._42 = 0.f;
            parentSkin._43 = 0.f;
        }
        combinedSkin = mul(combinedSkin, parentSkin);

        parent = InputBone[parent].iParentIndex;
    }

    combinedOriginal = mul(combinedOriginal, g_PreTransformMatrix);
    combinedSkin = mul(combinedSkin, g_PreTransformMatrix);

    if (g_fBlendRatio > 0.f)
    {
        float4x4 prevLocal = PrevLocalMatrix[bone].BoneLocalTransformMatrix;
        float4x4 prevComb = PrevLocalMatrix[bone].BoneCombinedTransformMatrix;

        localSkin = lerp(prevLocal, localSkin, g_fBlendRatio);
        combinedSkin = lerp(prevComb, combinedSkin, g_fBlendRatio);
    }

    g_CombinedOut[bone].BoneLocalTransformMatrix = localSkin;
    g_CombinedOut[bone].BoneCombinedTransformMatrix = combinedSkin;

    if (bone == g_iRootIndex)
    {
        g_RootOut[0].BoneLocalTransformMatrix = localOriginal;
        g_RootOut[0].BoneCombinedTransformMatrix = combinedOriginal;
    }
}
