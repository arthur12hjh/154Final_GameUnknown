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
    float fX = vRotation.x;
    float fY = vRotation.y;
    float fZ = vRotation.z;
    float fW = vRotation.w;
    
    float fX_X = fX * fX;
    float fX_Y = fX * fY;
    float fX_Z = fX * fZ;
    float fX_W = fX * fW;
    float fY_Y = fY * fY;
    float fY_Z = fY * fZ;
    float fY_W = fY * fW;
    float fZ_Z = fZ * fZ;
    float fZ_W = fZ * fW;
    float fW_W = fW * fW;
    
    return float4x4(
        1.f - 2.f * (fY_Y + fZ_Z), 2.f * (fX_Y + fZ_W), 2.f * (fX_Z - fY_W), 0.f,
        2.f * (fX_Y - fZ_W), 1.f - 2.f * (fX_X + fZ_Z), 2.f * (fY_Z + fX_W), 0.f,
        2.f * (fX_Z + fY_W), 2.f * (fY_Z - fX_W), 1.f - 2.f * (fX_X + fY_Y), 0.f,
        0.f, 0.f, 0.f, 1.f
    );
}

float4 QuaternionSlerp(float4 q1, float4 q2, float t)
{
    float fCosTheta = dot(q1, q2);
    
    if (fCosTheta < 0.0f)
    {
        q2 = -q2;
        fCosTheta = -fCosTheta;
    }
    
    if (fCosTheta > 0.9995f)
    {
        return normalize(lerp(q1, q2, t));
    }

    float theta = acos(fCosTheta);
    float sinTheta = sin(theta);

    float w1 = sin((1.0f - t) * theta) / sinTheta;
    float w2 = sin(t * theta) / sinTheta;

    return q1 * w1 + q2 * w2;
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

// Const Buffer
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
    uint g_BatchOffset;
}

StructuredBuffer<BoneInfo> InputBone : register(t0);
StructuredBuffer<ChannelInfo> InputChannel : register(t1);
StructuredBuffer<KeyFrameInfo> InputKeyFrame : register(t2);
StructuredBuffer<BoneTransformMatrixOut> InputLocalMatrix : register(t3);
RWStructuredBuffer<BoneTransformMatrixOut> g_CombinedOut : register(u0);

[numthreads(128, 1, 1)]
void CombinedMatrices(uint3 Gid : SV_GroupID,
                   uint3 DTid : SV_DispatchThreadID,
                   uint3 GTid : SV_GroupThreadID,
                   uint GI : SV_GroupIndex)
{
    uint iBoneIndex = DTid.x;
    if (iBoneIndex >= g_iNumBones)
        return;

    // boneIndex -> ChannelInfo
    ChannelInfo CurrentChannel = InputChannel[iBoneIndex];

    // Model에서는, Local과 Combined를 나눠서 작업해줬지만, 그럼 GPU는 투패스가 돼버린다. ㅈㄴ귀찮다 ㅇㅇ;
    // 그래서 우리는 원패스로 해줄거다
    float4x4 BoneLocalTransformMatrix;
    
    // 이 본은 애니메이션 채널이 없으므로, CPU에서 미리 써둔 로컬행렬을 그대로 사용한다.
    if (CurrentChannel.iNumKeyFrames == 0)
    {
        BoneLocalTransformMatrix = InputLocalMatrix[iBoneIndex].BoneLocalTransformMatrix;
        g_CombinedOut[iBoneIndex].BoneLocalTransformMatrix = BoneLocalTransformMatrix;
    }
    else
    {
        vector vScale;
        vector vRotation;
        vector vTranslation;

        uint baseIndex = CurrentChannel.iKeyFrameOffset;
        uint localKeyFrameIndex = CurrentChannel.iCurrentKeyFrameIndex;

        uint iCurrentKeyFrameIndex = baseIndex + localKeyFrameIndex;
        uint iLastKeyFrameIndex = baseIndex + (CurrentChannel.iNumKeyFrames - 1);

        uint iNextKeyFrameIndex = min(iCurrentKeyFrameIndex + 1, iLastKeyFrameIndex);
    
    
        float t = g_fCurrentTrackPosition;

        if (g_bIsLoop != 0 && g_fDuration > 0.0f)
        {
        
            t = fmod(t, g_fDuration);
            if (t < 0.0f)
                t += g_fDuration;
        }

    // 마지막 키프레임 이후면 그냥 마지막 키프레임 고정
        if (t >= InputKeyFrame[iLastKeyFrameIndex].fTrackPosition)
        {
            vScale = vector(InputKeyFrame[iLastKeyFrameIndex].vScale, 1.f);
            vRotation = InputKeyFrame[iLastKeyFrameIndex].vRotation;
            vTranslation = vector(InputKeyFrame[iLastKeyFrameIndex].vTranslation, 1.f);
        }
        else
        {
            float3 vSourScale, vDestScale;
            float4 vSourRotation, vDestRotation;
            float3 vSourTranslation, vDestTranslation;

            vSourScale = InputKeyFrame[iCurrentKeyFrameIndex].vScale;
            vDestScale = InputKeyFrame[iNextKeyFrameIndex].vScale;
            vSourRotation = InputKeyFrame[iCurrentKeyFrameIndex].vRotation;
            vDestRotation = InputKeyFrame[iNextKeyFrameIndex].vRotation;
            vSourTranslation = InputKeyFrame[iCurrentKeyFrameIndex].vTranslation;
            vDestTranslation = InputKeyFrame[iNextKeyFrameIndex].vTranslation;

            float fStartTime = InputKeyFrame[iCurrentKeyFrameIndex].fTrackPosition;
            float fEndTime = InputKeyFrame[iNextKeyFrameIndex].fTrackPosition;
            float fDuration = max(fEndTime - fStartTime, 0.0001f);

            float fRatio = saturate((t - fStartTime) / fDuration);
            float fSourWeight = 1.0f - fRatio;
            float fDestWeight = fRatio;

            vScale = vector(lerp(vSourScale, vDestScale, fRatio), 1.f);

        // 쿼터니언 Slerp 사용
            vRotation = QuaternionSlerp(vSourRotation, vDestRotation, fRatio);

            vTranslation = vector(lerp(vSourTranslation, vDestTranslation, fRatio), 1.f);
        }

        float4x4 matScale = MakeScaleMatrix(vScale);
        float4x4 matRotation = MakeRotationMatrix(vRotation);
        float4x4 matTranslation = MakeTranslationMatrix(vTranslation);
        
        BoneLocalTransformMatrix = mul(mul(matScale, matRotation), matTranslation);
        g_CombinedOut[iBoneIndex].BoneLocalTransformMatrix = BoneLocalTransformMatrix;
    }

    // 이렇게 하면 각 Bone들의 LocalTransformMatrix가 나온다. 이제 그냥 InputBone의 부모 인덱스 매트릭스를 곱해주면 된다.
    if(InputBone[iBoneIndex].iParentIndex == -1)
    {
        g_CombinedOut[iBoneIndex].BoneCombinedTransformMatrix = mul(BoneLocalTransformMatrix, g_PreTransformMatrix);
    }
    else
    {
        g_CombinedOut[iBoneIndex].BoneCombinedTransformMatrix =
        mul(BoneLocalTransformMatrix, g_CombinedOut[InputBone[iBoneIndex].iParentIndex].BoneCombinedTransformMatrix);

    }
    
}