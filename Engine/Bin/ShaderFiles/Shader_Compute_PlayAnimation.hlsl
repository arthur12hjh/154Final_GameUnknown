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
    
    // 반대방향 보정
    if (fCosTheta < 0.0f)
    {
        q2 = -q2;
        fCosTheta = -fCosTheta;
    }
    
    // 거의 같은 방향이면 Lerp 사용
    if (fCosTheta > 0.9995f)
    {
        return normalize(lerp(q1, q2, t));
    }

    float theta = acos(fCosTheta);
    float sinTheta = sin(theta);

    float w1 = sin((1.0f - t) * theta) / sinTheta;
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
}

StructuredBuffer<BoneInfo> InputBone : register(t0);
StructuredBuffer<ChannelInfo> InputChannel : register(t1);
StructuredBuffer<KeyFrameInfo> InputKeyFrame : register(t2);
StructuredBuffer<BoneTransformMatrixOut> InputLocalMatrix : register(t3);
RWStructuredBuffer<BoneTransformMatrixOut> g_CombinedOut : register(u0);
RWStructuredBuffer<BoneTransformMatrixOut> g_RootOut : register(u1);

uint ComputeKeyFrameIndex(uint boneIndex, float t)
{
    ChannelInfo ch = InputChannel[boneIndex];

    if (ch.iNumKeyFrames <= 1)
        return 0;

    uint start = ch.iKeyFrameOffset;
    uint end = start + ch.iNumKeyFrames - 1;

    // 마지막 프레임 체크
    if (t >= InputKeyFrame[end].fTrackPosition)
        return ch.iNumKeyFrames - 1;

    // 선형 탐색
    for (uint k = 0; k < ch.iNumKeyFrames - 1; k++)
    {
        float t0 = InputKeyFrame[start + k].fTrackPosition;
        float t1 = InputKeyFrame[start + k + 1].fTrackPosition;

        if (t >= t0 && t < t1)
            return k;
    }

    return ch.iNumKeyFrames - 1;
}


float4x4 ComputeLocalMatrixForBone(uint iBoneIndex, float t)
{
    ChannelInfo channel = InputChannel[iBoneIndex];

    // 키프레임이 없거나 1개 이하인 경우 → 애니메이션 없이 초기 로컬 행렬 사용
    if (channel.iNumKeyFrames <= 1)
    {
        return InputLocalMatrix[iBoneIndex].BoneLocalTransformMatrix;
    }

    uint baseIndex = channel.iKeyFrameOffset;
    uint localIndex = ComputeKeyFrameIndex(iBoneIndex, t);
    uint lastIndex = baseIndex + (channel.iNumKeyFrames - 1);

    // CPU에서 이미 인덱스는 관리하므로, 안전용으로 클램프만
    uint currentIndex = baseIndex + localIndex;
    currentIndex = min(currentIndex, lastIndex);
    uint nextIndex = min(currentIndex + 1, lastIndex);

    // 마지막 키프레임 이후면 마지막 키프레임 고정
    if (t >= InputKeyFrame[lastIndex].fTrackPosition)
    {
        float3 vScale = InputKeyFrame[lastIndex].vScale;
        float4 vRotation = InputKeyFrame[lastIndex].vRotation;
        float3 vTranslation = InputKeyFrame[lastIndex].vTranslation;

        float4x4 matScale = MakeScaleMatrix(float4(vScale, 1.f));
        float4x4 matRotation = MakeRotationMatrix(vRotation);
        float4x4 matTranslation = MakeTranslationMatrix(float4(vTranslation, 1.f));

        return mul(mul(matScale, matRotation), matTranslation); // S * R * T
    }
    else
    {
        float3 vSourScale = InputKeyFrame[currentIndex].vScale;
        float3 vDestScale = InputKeyFrame[nextIndex].vScale;
        float4 vSourRotation = InputKeyFrame[currentIndex].vRotation;
        float4 vDestRotation = InputKeyFrame[nextIndex].vRotation;
        float3 vSourTranslation = InputKeyFrame[currentIndex].vTranslation;
        float3 vDestTranslation = InputKeyFrame[nextIndex].vTranslation;

        float fStartTime = InputKeyFrame[currentIndex].fTrackPosition;
        float fEndTime = InputKeyFrame[nextIndex].fTrackPosition;
        float fDuration = max(fEndTime - fStartTime, 0.0001f);

        float fRatio = saturate((t - fStartTime) / fDuration);

        float3 vScale = lerp(vSourScale, vDestScale, fRatio);
        float4 vRotation = QuaternionSlerp(vSourRotation, vDestRotation, fRatio);
        float3 vTranslation = lerp(vSourTranslation, vDestTranslation, fRatio);

        float4x4 matScale = MakeScaleMatrix(float4(vScale, 1.f));
        float4x4 matRotation = MakeRotationMatrix(vRotation);
        float4x4 matTranslation = MakeTranslationMatrix(float4(vTranslation, 1.f));

        return mul(mul(matScale, matRotation), matTranslation);
    }
}
[numthreads(128, 1, 1)]
void CombinedMatrices(uint3 Gid : SV_GroupID,
                      uint3 DTid : SV_DispatchThreadID,
                      uint3 GTid : SV_GroupThreadID,
                      uint GI : SV_GroupIndex)
{
    uint iBoneIndex = DTid.x;
    if (iBoneIndex >= g_iNumBones)
        return;

    // 트랙 위치 정리
    float t = g_fCurrentTrackPosition;
    if (g_bIsLoop != 0 && g_fDuration > 0.0f)
    {
        t = fmod(t, g_fDuration);
        if (t < 0.0f)
            t += g_fDuration;
    }

    // 1) 원본 LocalMatrix 계산
    float4x4 BoneLocalOriginal = ComputeLocalMatrixForBone(iBoneIndex, t);

    // 스키닝용 로컬은 Root일 경우 Translation 초기화
    float4x4 BoneLocalForSkin = BoneLocalOriginal;
    if (iBoneIndex == g_iRootIndex)
    {
        BoneLocalForSkin._41 = 0;
        BoneLocalForSkin._42 = 0;
        BoneLocalForSkin._43 = 0;
    }

    // 2) Combined 계산 시작
    float4x4 CombinedOriginal = BoneLocalOriginal;
    float4x4 CombinedSkin = BoneLocalForSkin;
    
    // 부모 체인을 타고 Combined 생성
    int parentIndex = InputBone[iBoneIndex].iParentIndex;
    while (parentIndex >= 0)
    {
        float4x4 PLocal = ComputeLocalMatrixForBone(parentIndex, t);
        CombinedOriginal = mul(CombinedOriginal, PLocal);

        float4x4 PLocalSkin = PLocal;
        if (parentIndex == g_iRootIndex)
        {
            PLocalSkin._41 = 0;
            PLocalSkin._42 = 0;
            PLocalSkin._43 = 0;
        }
        CombinedSkin = mul(CombinedSkin, PLocalSkin);

        parentIndex = InputBone[parentIndex].iParentIndex;
    }

    CombinedOriginal = mul(CombinedOriginal, g_PreTransformMatrix);
    CombinedSkin = mul(CombinedSkin, g_PreTransformMatrix);

    // 3) GPU 스키닝용 버퍼(u1)에 기록
    g_CombinedOut[iBoneIndex].BoneLocalTransformMatrix = BoneLocalForSkin;
    g_CombinedOut[iBoneIndex].BoneCombinedTransformMatrix = CombinedSkin;

    // 4) CPU RootMotion 용은 RootBone만 저장
    if (iBoneIndex == g_iRootIndex)
    {
        g_RootOut[0].BoneLocalTransformMatrix = BoneLocalOriginal;
        g_RootOut[0].BoneCombinedTransformMatrix = CombinedOriginal;
    }
}