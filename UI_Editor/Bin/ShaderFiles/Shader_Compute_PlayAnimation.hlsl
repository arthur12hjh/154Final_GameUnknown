// fSign의 부호를 fSource의 부호로 대입하는 함수
// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/ 의 Alternative Method 참고
float CopySign(float fSource, float fSign)
{
    if (fSource < 0.f)
        fSource *= -1.f;
    
    if (fSign < 0.f)
        return fSource *= -1.f;
    else
        return fSource;
}

// 크기값 받아서 행렬 만드는 함수
float4x4 MakeScaleMatrix(float4 vScale)
{
    return float4x4(
        vScale.x, 0.f, 0.f, 0.f,
        0.f, vScale.y, 0.f, 0.f,
        0.f, 0.f, vScale.z, 0.f,
        0.f, 0.f, 0.f, 1.f
    );
}

// 위치 행렬
float4x4 MakeTranslationMatrix(float4 vTranslation)
{
    return float4x4(
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        vTranslation.x, vTranslation.y, vTranslation.z, 1.f
    );
}

// 회전행렬
float4x4 MakeRotationMatrix(float4 vRotation)
{
    float fX = vRotation.x;
    float fY = vRotation.y;
    float fZ = vRotation.z;
    float fW = vRotation.w;

    float fMulXX = fX * fX;
    float fMulXY = fX * fY;
    float fMulXZ = fX * fZ;
    float fMulXW = fX * fW;
    float fMulYY = fY * fY;
    float fMulYZ = fY * fZ;
    float fMulYW = fY * fW;
    float fMulZZ = fZ * fZ;
    float fMulZW = fZ * fW;

    return float4x4(
        1.f - 2.f * (fMulYY + fMulZZ), 2.f * (fMulXY + fMulZW), 2.f * (fMulXZ - fMulYW), 0.f,
        2.f * (fMulXY - fMulZW), 1.f - 2.f * (fMulXX + fMulZZ), 2.f * (fMulYZ + fMulXW), 0.f,
        2.f * (fMulXZ + fMulYW), 2.f * (fMulYZ - fMulXW), 1.f - 2.f * (fMulXX + fMulYY), 0.f,
        0.f, 0.f, 0.f, 1.f
    );
}

// Slerp
float4 QuaternionSlerp(float4 q1, float4 q2, float fRatio)
{
    // 두 벡터를 내적해서 코사인 세타를 구한다.
    float fCosTheta = dot(q1, q2);
    
    // 각도가 반대면, 반대로 곱해준다.
    if (fCosTheta < 0.0f)
    {
        q2 = -q2;
        fCosTheta = -fCosTheta;
    }
    
    // DOT_THRESHOLD
    if (fCosTheta > 0.9995f)
    {
        float4 fResult = q1 + fRatio * (q2 - q1);
        return normalize(fResult);
    }
    
    
    // q1 * cosTheta + (q2 - q1 * fCosTheta) * fSinTheta
    // fSinTheta를 구해주자.
    fCosTheta = clamp(fCosTheta, -1.f, 1.f);
    float fTheta = acos(fCosTheta);
    float fSinTheta = sin(fTheta);
    
    return normalize(q1 * sin((1.f - fRatio) * fTheta) / fSinTheta + q2 * sin(fRatio * fTheta) / fSinTheta);
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
// 애니메이션 보간용. << 여기서 문제가 터진다.
StructuredBuffer<BoneTransformMatrixOut> PrevLocalMatrix : register(t4);

RWStructuredBuffer<BoneTransformMatrixOut> g_CombinedOut : register(u0);
RWStructuredBuffer<BoneTransformMatrixOut> g_RootOut : register(u1);

// 키프레임 오프셋을 기반으로 현재 본에 갱신할 키프레임 위치를 계산한다.
uint ComputeKeyFrameIndex(uint iBoneIndex, float fTime)
{
    ChannelInfo ChannelDesc = InputChannel[iBoneIndex];
    if (ChannelDesc.iNumKeyFrames <= 1)
        return 0;

    uint iStart = ChannelDesc.iKeyFrameOffset;
    uint iEnd = iStart + ChannelDesc.iNumKeyFrames - 1;

    if (fTime >= InputKeyFrame[iEnd].fTrackPosition)
        return ChannelDesc.iNumKeyFrames - 1;

    for (uint i = 0; i < ChannelDesc.iNumKeyFrames - 1; ++i)
    {
        float fTime0 = InputKeyFrame[iStart + i].fTrackPosition;
        float fTime1 = InputKeyFrame[iStart + i + 1].fTrackPosition;

        if (fTime >= fTime0 && fTime < fTime1)
            return i;
    }

    return ChannelDesc.iNumKeyFrames - 1;
}

// 로컬행렬을 계산한다.
// CombinedMatrix를 부모를 타고 올라가 갱신해주는 방식으로 진행할 것이다. < ㅈㄴ 비효율적인거 아님? : 그래도 GPU가 이긴다.
float4x4 ComputeLocalMatrix(uint iBoneIndex, float fTime)
{
    ChannelInfo ChannelDesc = InputChannel[iBoneIndex];

    if (ChannelDesc.iNumKeyFrames <= 1)
        return InputLocalMatrix[iBoneIndex].BoneLocalTransformMatrix;

    uint iOffset = ChannelDesc.iKeyFrameOffset;
    uint iCurrentIndex = iOffset + ComputeKeyFrameIndex(iBoneIndex, fTime);
    uint iNextIndex = min(iCurrentIndex + 1, iOffset + ChannelDesc.iNumKeyFrames - 1);

    float fCurrentTime = InputKeyFrame[iCurrentIndex].fTrackPosition;
    float fNextTime = InputKeyFrame[iNextIndex].fTrackPosition;
    float fTimeDelta = max(fNextTime - fCurrentTime, 0.0001f);
    
    float fRatio = saturate((fTime - fCurrentTime) / fTimeDelta);

    float3 vScale0 = InputKeyFrame[iCurrentIndex].vScale;
    float3 vScale1 = InputKeyFrame[iNextIndex].vScale;

    float4 vRotation0 = InputKeyFrame[iCurrentIndex].vRotation;
    float4 vRotation1 = InputKeyFrame[iNextIndex].vRotation;

    float3 vTranslation0 = InputKeyFrame[iCurrentIndex].vTranslation;
    float3 vTranslation1 = InputKeyFrame[iNextIndex].vTranslation;

    // 순서는 (Scale, Rotation), Translation
    float3 vCalculatedScale = lerp(vScale0, vScale1, fRatio);
    float4 vCalculatedRotation = QuaternionSlerp(vRotation0, vRotation1, fRatio);
    float3 vCalculatedTranslation = lerp(vTranslation0, vTranslation1, fRatio);

    float4x4 matScale = MakeScaleMatrix(float4(vCalculatedScale, 1.f));
    float4x4 matRotation = MakeRotationMatrix(vCalculatedRotation);
    float4x4 matTranslation = MakeTranslationMatrix(float4(vCalculatedTranslation, 1.f));

    return mul(mul(matScale, matRotation), matTranslation);
}

float3 ScaleLerp(float4x4 matSrc, float4x4 matDst, float fRatio)
{
    float3 vSrcScale, vDstScale;
    
    vSrcScale.x = length(matSrc[0].xyz);
    vSrcScale.y = length(matSrc[1].xyz);
    vSrcScale.z = length(matSrc[2].xyz);
    
    vDstScale.x = length(matDst[0].xyz);
    vDstScale.y = length(matDst[1].xyz);
    vDstScale.z = length(matDst[2].xyz);
    
    return lerp(vSrcScale, vDstScale, fRatio);
}
float4 RotationLerp(float4x4 matSrc, float4x4 matDst, float fRatio)
{
    matSrc[0].xyz = normalize(matSrc[0].xyz);
    matSrc[1].xyz = normalize(matSrc[1].xyz);
    matSrc[2].xyz = normalize(matSrc[2].xyz);
    
    matDst[0].xyz = normalize(matDst[0].xyz);
    matDst[1].xyz = normalize(matDst[1].xyz);
    matDst[2].xyz = normalize(matDst[2].xyz);
    
    float4 vSrcRotation, vDstRotation;
    
    vSrcRotation.w = sqrt(max(0.f, 1.f + matSrc[0][0] + matSrc[1][1] + matSrc[2][2])) * 0.5f;
    vSrcRotation.x = sqrt(max(0.f, 1.f + matSrc[0][0] - matSrc[1][1] - matSrc[2][2])) * 0.5f;
    vSrcRotation.y = sqrt(max(0.f, 1.f - matSrc[0][0] + matSrc[1][1] - matSrc[2][2])) * 0.5f;
    vSrcRotation.z = sqrt(max(0.f, 1.f - matSrc[0][0] - matSrc[1][1] + matSrc[2][2])) * 0.5f;
    
    vSrcRotation.x = CopySign(vSrcRotation.x, matSrc[2][1] - matSrc[1][2]);
    vSrcRotation.y = CopySign(vSrcRotation.y, matSrc[0][2] - matSrc[2][0]);
    vSrcRotation.z = CopySign(vSrcRotation.z, matSrc[1][0] - matSrc[0][1]);
    
    vDstRotation.w = sqrt(max(0.f, 1.f + matDst[0][0] + matDst[1][1] + matDst[2][2])) * 0.5f;
    vDstRotation.x = sqrt(max(0.f, 1.f + matDst[0][0] - matDst[1][1] - matDst[2][2])) * 0.5f;
    vDstRotation.y = sqrt(max(0.f, 1.f - matDst[0][0] + matDst[1][1] - matDst[2][2])) * 0.5f;
    vDstRotation.z = sqrt(max(0.f, 1.f - matDst[0][0] - matDst[1][1] + matDst[2][2])) * 0.5f;
    
    vDstRotation.x = CopySign(vDstRotation.x, matDst[2][1] - matDst[1][2]);
    vDstRotation.y = CopySign(vDstRotation.y, matDst[0][2] - matDst[2][0]);
    vDstRotation.z = CopySign(vDstRotation.z, matDst[1][0] - matDst[0][1]);
    
    vSrcRotation.w *= -1.f;
    vDstRotation.w *= -1.f;
 
    vSrcRotation = normalize(vSrcRotation);
    vDstRotation = normalize(vDstRotation);
    
    return QuaternionSlerp(vSrcRotation, vDstRotation, fRatio);
}

float3 TranslationLerp(float4x4 matSrc, float4x4 matDst, float fRatio)
{
    float3 vSrcTranslation, vDstTranslation;
    
    vSrcTranslation = matSrc[3].xyz;
    vDstTranslation = matDst[3].xyz;
    
    return lerp(vSrcTranslation, vDstTranslation, fRatio);
}

[numthreads(128, 1, 1)]
void CombinedMatrices(uint3 gid : SV_GroupID,
                      uint3 dtid : SV_DispatchThreadID,
                      uint3 gtid : SV_GroupThreadID,
                      uint gi : SV_GroupIndex)
{
    uint iBoneIndex = dtid.x;
    if (iBoneIndex >= g_iNumBones)
        return;

    float fCurrentTime = g_fCurrentTrackPosition;
    if (g_bIsLoop != 0 && g_fDuration > 0.f)
    {
        fCurrentTime = fmod(fCurrentTime, g_fDuration);
        if (fCurrentTime < 0.f)
            fCurrentTime += g_fDuration;
    }
    
    float4x4 matLocalOriginal = ComputeLocalMatrix(iBoneIndex, fCurrentTime);
    float4x4 matLocalSkin = matLocalOriginal;

    if (iBoneIndex == g_iRootIndex)
    {
        matLocalSkin._41 = 0.f;
        matLocalSkin._42 = 0.f;
        matLocalSkin._43 = 0.f;
    }
    
    
    if (g_fBlendRatio > 0.f)
    {
        float4x4 matPrevLocal = PrevLocalMatrix[iBoneIndex].BoneLocalTransformMatrix;

        float3 vScaleLocal = ScaleLerp(matPrevLocal, matLocalSkin, g_fBlendRatio);
        float4 vRotationLocal = RotationLerp(matPrevLocal, matLocalSkin, g_fBlendRatio);
        float3 vTranslationLocal = TranslationLerp(matPrevLocal, matLocalSkin, g_fBlendRatio);

        float4x4 S = MakeScaleMatrix(float4(vScaleLocal, 1.f));
        float4x4 R = MakeRotationMatrix(vRotationLocal);
        float4x4 T = MakeTranslationMatrix(float4(vTranslationLocal, 1.f));

        matLocalSkin = mul(mul(S, R), T);
    }
    
    float4x4 matCombinedOriginal = matLocalOriginal;
    float4x4 matCombinedSkin = matLocalSkin;

    int iParentIndex = InputBone[iBoneIndex].iParentIndex;
    
    // 요주의 인물. GPU는 CPU와 다르게 선형 연산이 보장되지 않기에 부모 본의 안전 여부를 알 수 없다.
    // 별 수 있나? 가지뻗듯이 계산을 해줘야한다.
    // 투패스로 바꿀경우 이 연산이 필요없어지기에, 최적화 1순위. 나중에 보간까지 수정해야해서 살짝 대공사 예정
    while (iParentIndex >= 0)
    {
        float4x4 matParentLocalOriginal = ComputeLocalMatrix(iParentIndex, fCurrentTime);
        float4x4 matParentLocalSkin = matParentLocalOriginal;

        if (iParentIndex == g_iRootIndex)
        {
            matParentLocalSkin._41 = 0.f;
            matParentLocalSkin._42 = 0.f;
            matParentLocalSkin._43 = 0.f;
        }

        if (g_fBlendRatio > 0.f)
        {
            float4x4 matPrevParentLocal = PrevLocalMatrix[iParentIndex].BoneLocalTransformMatrix;

            float3 vScaleParent = ScaleLerp(matPrevParentLocal, matParentLocalSkin, g_fBlendRatio);
            float4 vRotationParent = RotationLerp(matPrevParentLocal, matParentLocalSkin, g_fBlendRatio);
            float3 vTranslationParent = TranslationLerp(matPrevParentLocal, matParentLocalSkin, g_fBlendRatio);

            float4x4 S = MakeScaleMatrix(float4(vScaleParent, 1.f));
            float4x4 R = MakeRotationMatrix(vRotationParent);
            float4x4 T = MakeTranslationMatrix(float4(vTranslationParent, 1.f));

            matParentLocalSkin = mul(mul(S, R), T);
        }
        
        matCombinedOriginal = mul(matCombinedOriginal, matParentLocalOriginal);
        matCombinedSkin = mul(matCombinedSkin, matParentLocalSkin);

        iParentIndex = InputBone[iParentIndex].iParentIndex;
    }
    
    matCombinedOriginal = mul(matCombinedOriginal, g_PreTransformMatrix);
    matCombinedSkin = mul(matCombinedSkin, g_PreTransformMatrix);
    
    g_CombinedOut[iBoneIndex].BoneLocalTransformMatrix = matLocalSkin;
    g_CombinedOut[iBoneIndex].BoneCombinedTransformMatrix = matCombinedSkin;

    if (iBoneIndex == g_iRootIndex)
    {
        g_RootOut[0].BoneLocalTransformMatrix = matLocalOriginal;
        g_RootOut[0].BoneCombinedTransformMatrix = matCombinedOriginal;
    }
}
