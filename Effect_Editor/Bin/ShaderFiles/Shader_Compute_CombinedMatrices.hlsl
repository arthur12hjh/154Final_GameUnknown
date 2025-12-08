struct BoneInfo
{
    int iParentIndex;
    int3 _padding;
};

struct BoneTransformMatrixCombined
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
StructuredBuffer<BoneTransformMatrixCombined> InputLocalMatrix : register(t1);

RWStructuredBuffer<BoneTransformMatrixCombined> g_CombinedOut : register(u0);
RWStructuredBuffer<BoneTransformMatrixCombined> g_RootOut : register(u1);

[numthreads(128, 1, 1)]
void CombinedMatrices(uint3 gid : SV_GroupID,
                      uint3 dtid : SV_DispatchThreadID,
                      uint3 gtid : SV_GroupThreadID,
                      uint gi : SV_GroupIndex)
{
    uint iBoneIndex = dtid.x;
    if (iBoneIndex >= g_iNumBones)
        return;
    
    // 현재 트랙 포지션을 받아와서, 현재 키프레임 인덱스를 찾아준다.
    float fCurrentTime = g_fCurrentTrackPosition;
    if (g_bIsLoop != 0 && g_fDuration > 0.f)
    {
        fCurrentTime = fmod(fCurrentTime, g_fDuration);
        if (fCurrentTime < 0.f)
            fCurrentTime += g_fDuration;
    }
    
    float4x4 matLocalOriginal = InputLocalMatrix[iBoneIndex].BoneLocalTransformMatrix;
    float4x4 matLocalSkin = InputLocalMatrix[iBoneIndex].BoneCombinedTransformMatrix;
    
    float4x4 matCombinedOriginal = matLocalOriginal;
    float4x4 matCombinedSkin = matLocalSkin;

    int iParentIndex = InputBone[iBoneIndex].iParentIndex;
    
    while (iParentIndex >= 0)
    {
        float4x4 matParentLocalOriginal = InputLocalMatrix[iParentIndex].BoneLocalTransformMatrix;
        float4x4 matParentLocalSkin = InputLocalMatrix[iParentIndex].BoneCombinedTransformMatrix;
        
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
