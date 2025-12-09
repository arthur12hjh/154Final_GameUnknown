#include "Model.h"

#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Material.h"
#include "Animation.h"
#include "Channel.h"
#include "ComputeShader.h"
#include "GameInstance.h"
#include "StringHelper.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& Prototype)
    : CComponent{ Prototype }
    , m_eType{ Prototype.m_eType }
    , m_iNumMeshes{ Prototype.m_iNumMeshes }
    , m_Meshes{ Prototype.m_Meshes }
    , m_iNumMaterials{ Prototype.m_iNumMaterials }
    , m_Materials{ Prototype.m_Materials }
    , m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
    , m_iNumAnimations{ Prototype.m_iNumAnimations }
    , m_GlobalOffsetMatrices{ Prototype.m_GlobalOffsetMatrices }
    , m_AnimationIndexMap{ Prototype.m_AnimationIndexMap }
    , m_pBoneSource{ nullptr }
    , m_pOutSource{ nullptr }
    , m_pPreBoneMatrices{ nullptr }
    , m_pOutReadBack{ nullptr }
    , m_pOutRootReadBack{nullptr }
    , m_pBoneMatricesSRV{ nullptr }
    , m_pPreBoneMatricesSRV{ nullptr }
    , m_pComputeShaderCom{ nullptr }
    , m_pCombinedMatrixComputeShaderCom{ nullptr }
{
    for (auto& pPrototypeBone : Prototype.m_Bones)
        m_Bones.push_back(pPrototypeBone->Clone());

    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);

    for (auto& pMaterial : m_Materials)
        Safe_AddRef(pMaterial);

    for (auto* pChannelBuffer : Prototype.m_pChannelBufferList)
    {
        Safe_AddRef(pChannelBuffer);
        m_pChannelBufferList.push_back(pChannelBuffer);
    }

    for (auto* pKeyFrameBuffer : Prototype.m_pKeyFrameBufferList)
    {
        Safe_AddRef(pKeyFrameBuffer);
        m_pKeyFrameBufferList.push_back(pKeyFrameBuffer);
    }

    for (auto& pPrototypeAnim : Prototype.m_Animations)
        m_Animations.push_back(pPrototypeAnim->Clone());



    memcpy(m_szBindTags, Prototype.m_szBindTags, sizeof(m_szBindTags));

}

_uint CModel::Get_Mesh_MaterialIndex(_uint iIdx) const
{
    return m_Meshes[iIdx]->Get_MaterialIndex();
}

const _char* CModel::Get_MeshName(_uint iIdx) const
{
    return m_Meshes[iIdx]->Get_Name();
}

const _char* CModel::Get_MaterialName(_uint iIdx) const
{
    return m_Materials[iIdx]->Get_Name();
}

_int CModel::Get_BoneIndex(const _char* pBoneName) const
{
    _int   iBoneIndex = {};

    auto   iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
        {
            if (true == pBone->Compare_Name(pBoneName))
                return true;

            ++iBoneIndex;

            return false;
        });

    if (iter == m_Bones.end())
        return -1;

    return iBoneIndex;
}

vector<class CBone*>* CModel::Get_Bones()
{
    return &m_Bones;
}

vector<class CMaterial*>* CModel::Get_Materials()
{
    return &m_Materials;
}

_uint CModel::Get_AnimationKeyFrameIndex() const
{
    return m_Animations[m_iCurrentAnimIndex]->Get_TrackPosition();
}

const _float4x4* CModel::Get_BoneMatrixPtr(const _char* pBoneName)
{
    AddCount_PartialBone(pBoneName);

    auto   iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
        {
            if (true == pBone->Compare_Name(pBoneName))
                return true;

            return false;
        });

    if (m_Bones.end() == iter)
        return nullptr;

    return (*iter)->Get_CombinedTransformationMatrixPtr();
}

void CModel::Attach_CombinedTransformationMatrix()
{
    for (auto& pBone : m_Bones)
    {
        pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
    }
}

void CModel::Copy_MeshBuffer(_uint iMeshNum, ID3D11Buffer** pVIBuffer, ID3D11Buffer** pIndexBuffer)
{
    *pVIBuffer = m_Meshes[iMeshNum]->GetVIBuffer();
    *pIndexBuffer = m_Meshes[iMeshNum]->GetIBBuffer();
}

_uint CModel::Get_MeshIndices(_uint iMeshNum)
{
    return m_Meshes[iMeshNum]->GetIndices();
}

_uint CModel::Get_MeshVertexStride(_uint iMeshNum)
{
    return m_Meshes[iMeshNum]->GetVertexStride();
}

DXGI_FORMAT CModel::Get_MeshIndexFormat(_uint iMeshNum)
{
    return m_Meshes[iMeshNum]->GetIndexFormat();
}

void CModel::Set_AnimationIndex(_int iAnimIndex, _bool isLoop, _float fLerpDuration, _bool bIsRestart, _float fEndTrackPosition, _float fStartTrackPosition, _bool isResetTrackPosition)
{
    if (m_iCurrentAnimIndex == iAnimIndex && bIsRestart == FALSE)
        return;

    if (m_iCurrentAnimIndex != -1 && m_pOutSource != nullptr && fLerpDuration != 0.f)
    {
        m_isLerp = TRUE;
        m_fBlendElapsed = 0.f;
        m_fBlendRatio = 0.f;
    }
    else
    {
        m_isLerp = FALSE;
        m_fBlendElapsed = 0.f;
        m_fBlendRatio = 0.f;
    }

    m_fEndTrackPosition = fEndTrackPosition;
    m_fStartTrackPosition = fStartTrackPosition;
    _float fPreTrackPosition = -1.f;
    if (FALSE == isResetTrackPosition)
    {
        fPreTrackPosition = m_Animations[m_iCurrentAnimIndex]->Get_fTrackPosition();
    }

    m_fBlendDuration = fLerpDuration;
    m_iCurrentAnimIndex = iAnimIndex;
    m_isLoop = isLoop;


    if (AnimationChanged)
        AnimationChanged(m_Animations[m_iCurrentAnimIndex]->Get_Name());

    m_Animations[m_iCurrentAnimIndex]->Reset();

    if (FALSE == isResetTrackPosition)
    {
        m_Animations[m_iCurrentAnimIndex]->Set_CurrentTrackPosition(fPreTrackPosition);
    }
    else
    {
        m_Animations[m_iCurrentAnimIndex]->Set_CurrentTrackPosition(m_fStartTrackPosition);
    }

    m_iFlagPreRootModified = ROOTFLAG_RESET;
    XMStoreFloat4x4(&m_PreRootMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_CurRootMatrix, XMMatrixIdentity());

    Bind_ChannelAndKeyFrameBuffer();

    m_GlobalBuffer.g_fBlendRatio = 0.f;

    return;
}

void CModel::Set_Animation(const _wstring& strAnimationTag, _bool isLoop, _float fAnimationPlayRate, _float fLerpDuration, _bool bIsRestart, _float fEndTrackPosition, _float fStartTrackPosition, _bool isResetTrackPosition)
{
    _char pName[MAX_PATH] = {};

    CStringHelper::ConvertWideToUTF(strAnimationTag.c_str(), pName);

    _uint iAnimIndex = Find_Animation(pName);
    m_fAnimationPlayRate = fAnimationPlayRate;

    if (-1 == iAnimIndex)
        return;

    if (m_iCurrentAnimIndex == iAnimIndex && bIsRestart == FALSE)
        return;

    if (m_iCurrentAnimIndex != -1 && m_pOutSource != nullptr && fLerpDuration != 0.f)
    {
        m_isLerp = TRUE;
        m_fBlendElapsed = 0.f;
        m_fBlendRatio = 0.f;
    }
    else
    {
        m_isLerp = FALSE;
        m_fBlendElapsed = 0.f;
        m_fBlendRatio = 0.f;
    }

    m_fEndTrackPosition = fEndTrackPosition;
    m_fStartTrackPosition = fStartTrackPosition;
    _float fPreTrackPosition = -1.f;
    if (FALSE == isResetTrackPosition)
    {
        fPreTrackPosition = m_Animations[m_iCurrentAnimIndex]->Get_fTrackPosition();
    }

    m_fBlendDuration = fLerpDuration;
    m_iCurrentAnimIndex = iAnimIndex;
    m_isLoop = isLoop;


    m_Animations[m_iCurrentAnimIndex]->Reset();

    if (FALSE == isResetTrackPosition)
    {
        m_Animations[m_iCurrentAnimIndex]->Set_CurrentTrackPosition(fPreTrackPosition);
    }
    else
    {
        m_Animations[m_iCurrentAnimIndex]->Set_CurrentTrackPosition(m_fStartTrackPosition);
    }

    m_iFlagPreRootModified = ROOTFLAG_RESET;
    XMStoreFloat4x4(&m_PreRootMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_CurRootMatrix, XMMatrixIdentity());

    Bind_ChannelAndKeyFrameBuffer();

    if (AnimationChanged)
        AnimationChanged(m_Animations[m_iCurrentAnimIndex]->Get_Name());


    return;
}

void CModel::Set_Animation(const _char* szAnimationTag, _bool isLoop, _float fAnimationPlayRate, _float fLerpDuration, _bool bIsRestart, _float fEndTrackPosition, _float fStartTrackPosition, _bool isResetTrackPosition)
{
    _uint iAnimIndex = Find_Animation(szAnimationTag);
    m_fAnimationPlayRate = fAnimationPlayRate;

    if (-1 == iAnimIndex)
        return;

    if (m_iCurrentAnimIndex == iAnimIndex && bIsRestart == FALSE)
        return;

    if (m_iCurrentAnimIndex != -1 && m_pOutSource != nullptr && fLerpDuration != 0.f)
    {
        m_isLerp = TRUE;
        m_fBlendElapsed = 0.f;
        m_fBlendRatio = 0.f;
    }
    else
    {
        m_isLerp = FALSE;
        m_fBlendElapsed = 0.f;
        m_fBlendRatio = 0.f;
    }

    m_fEndTrackPosition = fEndTrackPosition;
    m_fStartTrackPosition = fStartTrackPosition;
    _float fPreTrackPosition = -1.f;
    if (FALSE == isResetTrackPosition)
    {
        fPreTrackPosition = m_Animations[m_iCurrentAnimIndex]->Get_fTrackPosition();
    }

    m_fBlendDuration = fLerpDuration;
    m_iCurrentAnimIndex = iAnimIndex;
    m_isLoop = isLoop;

    m_Animations[m_iCurrentAnimIndex]->Reset();

    if (FALSE == isResetTrackPosition)
    {
        m_Animations[m_iCurrentAnimIndex]->Set_CurrentTrackPosition(fPreTrackPosition);
    }
    else
    {
        m_Animations[m_iCurrentAnimIndex]->Set_CurrentTrackPosition(m_fStartTrackPosition);
    }

    m_iFlagPreRootModified = ROOTFLAG_RESET;
    XMStoreFloat4x4(&m_PreRootMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_CurRootMatrix, XMMatrixIdentity());

    Bind_ChannelAndKeyFrameBuffer();

    if (AnimationChanged)
        AnimationChanged(m_Animations[m_iCurrentAnimIndex]->Get_Name());


    return;
}

HRESULT CModel::Initialize_AnimationIndexMap()
{
    m_AnimationIndexMap.clear();

    _uint iAnimationIndex = 0;
    for (auto& pAnimation : m_Animations)
    {
        m_AnimationIndexMap.emplace(pAnimation->Get_Name(), iAnimationIndex);;
        ++iAnimationIndex;
    }

    return S_OK;
}

HRESULT CModel::Initialize_AnimationBufferResource()
{
    // 1) 다 밀어주기
    for (auto& pChannelBuffer : m_pChannelBufferList)
        Safe_Release(pChannelBuffer);
    m_pChannelBufferList.clear();
    
    for (auto& pKeyFrameBuffer : m_pKeyFrameBufferList)
        Safe_Release(pKeyFrameBuffer);
    m_pKeyFrameBufferList.clear();

    // 키프레임 수
    _uint iMaxNumKeyFrames = 0;

    for (auto& pAnim : m_Animations)
    {
        auto pChannels = pAnim->Get_vChannels();
        if (pChannels == nullptr)
            continue;

        _uint iTotal = 0;
        for (auto& pChannel : *pChannels)
            iTotal += pChannel->Get_NumKeyFrames();

        if (iTotal > iMaxNumKeyFrames)
            iMaxNumKeyFrames = iTotal;
    }

    // 채널 수
    _uint iNumData = (_uint)m_Bones.size();
    iNumData = max(iNumData, iMaxNumKeyFrames);
    if (iNumData == 0)
        return S_OK;


    // 컴퓨트 셰이더 최종 최적화 초식.. 버퍼로 보관하기
    // 메모리 손해 아니냐고? 프레임이 이난리인데 메모리가 중하냐!
    for (_uint iCurrentAnimationIndex = 0; iCurrentAnimationIndex < m_Animations.size(); ++iCurrentAnimationIndex)
    {
        vector<COMPUTE_CHANNELINFO> vChannelInfos(iNumData);
        vector<COMPUTE_KEYFRAMEINFO> vKeyFrameInfos;
        vKeyFrameInfos.reserve(iNumData);

        auto pChannels = m_Animations[iCurrentAnimationIndex]->Get_vChannels();
        auto& BoneToChannelMappingList = m_Animations[iCurrentAnimationIndex]->Get_BoneToChannelMappingLists();

        _uint iKeyFrameOffset = 0;

        for (_uint i = 0; i < iNumData; ++i)
        {
            if (i >= m_Bones.size())
            {
                // 여기는 “padding 영역” – 유효한 본 없음
                vChannelInfos[i].iBoneIndex = 0;
                vChannelInfos[i].iNumKeyFrames = 0;
                vChannelInfos[i].iCurrentKeyFrameIndex = 0;
                vChannelInfos[i].iKeyFrameOffset = 0;
                continue;
            }

            vChannelInfos[i].iBoneIndex = i;

            _int iChannelIndex = BoneToChannelMappingList[i];

            // 매핑된 채널이 없는 본
            if (iChannelIndex < 0)
            {
                vChannelInfos[i].iNumKeyFrames = 0;
                vChannelInfos[i].iCurrentKeyFrameIndex = 0;
                vChannelInfos[i].iKeyFrameOffset = 0;
                continue;
            }

            // 채널 인덱스가 실제 데이터 범위를 넘는 경우 (예방)
            if (iChannelIndex >= (_int)pChannels->size())
            {
                vChannelInfos[i].iNumKeyFrames = 0;
                vChannelInfos[i].iCurrentKeyFrameIndex = 0;
                vChannelInfos[i].iKeyFrameOffset = 0;
                continue;
            }

            CChannel* pChannel = (*pChannels)[iChannelIndex];

            // 안전 장치 – pChannel이 null일 경우
            if (pChannel == nullptr)
            {
                vChannelInfos[i].iNumKeyFrames = 0;
                vChannelInfos[i].iCurrentKeyFrameIndex = 0;
                vChannelInfos[i].iKeyFrameOffset = 0;
                continue;
            }

            // 이 본이 가진 키프레임 개수
            _uint iNumKeyFrames = pChannel->Get_NumKeyFrames();
            vChannelInfos[i].iNumKeyFrames = iNumKeyFrames;

            // 현재 키프레임 인덱스(CAnimation이 관리하는 것)
            vChannelInfos[i].iCurrentKeyFrameIndex =
                m_Animations[iCurrentAnimationIndex]->Get_AnimationKeyFrameIndex(iChannelIndex);

            // KeyFrame 버퍼 안에서 이 본의 키 시작 위치
            vChannelInfos[i].iKeyFrameOffset = iKeyFrameOffset;

            // 키프레임 데이터 밀어넣기
            for (_uint j = 0; j < iNumKeyFrames; ++j)
            {
                const KEYFRAME& KF = pChannel->Get_KeyFrame(j);

                COMPUTE_KEYFRAMEINFO OutKF = {};
                OutKF.vScale = KF.vScale;
                OutKF.padding01 = 0.f;
                OutKF.vRotation = KF.vRotation;
                OutKF.vTranslation = KF.vTranslation;
                OutKF.fTrackPosition = KF.fTrackPosition;

                vKeyFrameInfos.push_back(OutKF);
            }

            iKeyFrameOffset += iNumKeyFrames;
        }

        if (vKeyFrameInfos.size() < iNumData)
        {
            COMPUTE_KEYFRAMEINFO pad{};
            pad.vScale = { 1.f, 1.f, 1.f };
            pad.padding01 = 0.f;
            pad.vRotation = _float4(0.f, 0.f, 0.f, 1.f); // 단위 쿼터니언
            pad.vTranslation = { 0.f, 0.f, 0.f };
            pad.fTrackPosition = 0.f;

            vKeyFrameInfos.resize(iNumData, pad);
        }

        ID3D11Buffer* pChannelBuffer = nullptr;
        ID3D11Buffer* pKeyFrameBuffer = nullptr;

        D3D11_BUFFER_DESC ChannelBufferDesc = {};
        ChannelBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        ChannelBufferDesc.ByteWidth = sizeof(COMPUTE_CHANNELINFO) * iNumData;
        ChannelBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        ChannelBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        ChannelBufferDesc.StructureByteStride = sizeof(COMPUTE_CHANNELINFO);

        D3D11_SUBRESOURCE_DATA ChannelSubResource{};
        ChannelSubResource.pSysMem = vChannelInfos.data();


        if (FAILED(m_pDevice->CreateBuffer(&ChannelBufferDesc, &ChannelSubResource, &pChannelBuffer)))
            return E_FAIL;

        m_pChannelBufferList.push_back(pChannelBuffer);

        D3D11_BUFFER_DESC KeyFrameBufferDesc = {};
        KeyFrameBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        KeyFrameBufferDesc.ByteWidth = sizeof(COMPUTE_KEYFRAMEINFO) * iNumData;
        KeyFrameBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        KeyFrameBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        KeyFrameBufferDesc.StructureByteStride = sizeof(COMPUTE_KEYFRAMEINFO);

        D3D11_SUBRESOURCE_DATA KeyFrameSubResource{};
        KeyFrameSubResource.pSysMem = vKeyFrameInfos.empty() ? nullptr : vKeyFrameInfos.data();

        if (FAILED(m_pDevice->CreateBuffer(
            &KeyFrameBufferDesc,
            vKeyFrameInfos.empty() ? nullptr : &KeyFrameSubResource,
            &pKeyFrameBuffer)))
            return E_FAIL;

        m_pKeyFrameBufferList.push_back(pKeyFrameBuffer);
    }        


    return S_OK;
}

HRESULT CModel::Import_Animations(vector<class CAnimation*>* pAnimations)
{
    if (nullptr == pAnimations)
        return E_FAIL;

    for (auto& pAnimation : *pAnimations)
    {
        Mapping_Animation(pAnimation);

        m_Animations.push_back(pAnimation);
        Safe_AddRef(pAnimation);
        ++m_iNumAnimations;
    }


    return S_OK;
}

HRESULT CModel::Import_Texture(_uint iMeshIndex, TEXTURE_TYPE eType, const _char* pTextureFilePath, const _char* pBindTag, _bool bIsSaved)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    _uint      iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    if (iMaterialIndex >= m_iNumMaterials)
        return E_FAIL;

    char   strTexturePath[MAX_PATH];

    char      szDrive[MAX_PATH] = {};
    char      szDir[MAX_PATH] = {};
    char      szFileName[MAX_PATH] = {};
    char      szEXT[MAX_PATH] = {};

    strcpy_s(strTexturePath, pTextureFilePath);

    _char      szTextureFilePath[MAX_PATH] = {};
    _splitpath_s(pTextureFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
    _splitpath_s(strTexturePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

    strcpy_s(szTextureFilePath, szDrive);
    strcat_s(szTextureFilePath, szDir);
    strcat_s(szTextureFilePath, szFileName);
    strcat_s(szTextureFilePath, szEXT);

    _tchar      szAbsolutePath[MAX_PATH] = {};
    MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, strlen(szTextureFilePath),
        szAbsolutePath, MAX_PATH);

    ID3D11ShaderResourceView* pSRV = { nullptr };

    HRESULT         hr = {};

    if (false == strcmp(".dds", szEXT))
        hr = CreateDDSTextureFromFile(m_pDevice, szAbsolutePath, nullptr, &pSRV);
    else if (false == strcmp(".tga", szEXT))
        hr = S_OK;
    else
        hr = CreateWICTextureFromFile(m_pDevice, szAbsolutePath, nullptr, &pSRV);

    if (FAILED(hr))
        return E_FAIL;

    if (pBindTag != nullptr)
        strcpy_s(m_szBindTags[static_cast<_uint>(eType)], pBindTag);

    if (FAILED(m_Materials[iMaterialIndex]->Import_Texture(Convert_TextureType(eType), pSRV)))
        return E_FAIL;
    if (0 == m_pModel->vMaterials[iMeshIndex].vNumSRVs[Convert_TextureType(eType)])
    {
        m_pModel->vMaterials[iMeshIndex].vNumSRVs[Convert_TextureType(eType)]++;
        m_pModel->vMaterials[iMeshIndex].strTexturePaths[Convert_TextureType(eType)].push_back(szTextureFilePath);
    }
    else
    {
        m_pModel->vMaterials[iMeshIndex].strTexturePaths[Convert_TextureType(eType)][0] = szTextureFilePath;
    }

#ifdef _DEBUG
    if (bIsSaved == TRUE)
        m_pGameInstance->WriteBinx(m_ModelFilePath, m_eType, &m_pModel);
#endif
    return S_OK;
}

HRESULT CModel::Change_BoneTag(const _char* szAfterBoneTag, const vector<string>& szTargetTagList)
{
    unordered_map<string, int> BoneNameMap;

    _uint iBoneIndex = 0;
    for (auto& pBone : m_Bones)
    {
        BoneNameMap.emplace(pBone->Get_Name(), iBoneIndex);;
        ++iBoneIndex;
    }

    auto iter = BoneNameMap.find(szAfterBoneTag);

    if (iter != BoneNameMap.end())
        return E_FAIL;


    _char szNodeName[MAX_PATH] = {};
    _bool bIsTagFound = FALSE;
    for (auto szTargetTag : szTargetTagList)
    {
        auto iterTarget = BoneNameMap.find(szTargetTag);
        if (iterTarget != BoneNameMap.end())
        {
            bIsTagFound = TRUE;
            strcpy_s(szNodeName, (*iterTarget).first.c_str());
            m_Bones[(*iterTarget).second]->Set_Name(szAfterBoneTag);
            
            break;
        }
    }

    if (FALSE == bIsTagFound)
        return E_FAIL;

    for (auto& pNode : m_pModel->vNodes)
    {
        if (strcmp(pNode.szName, szNodeName) == 0)
        {
            strcpy_s(pNode.szName, szAfterBoneTag);
            break;
        }
    }

#ifdef _DEBUG
    m_pGameInstance->WriteBinx(m_ModelFilePath, m_eType, &m_pModel);
#endif
    return S_OK;
}

HRESULT CModel::AddCount_PartialBone(const _char* pBoneName)
{
    if (!pBoneName)
        return E_FAIL;

    _int iBoneIndex = Get_BoneIndex(pBoneName);
    if (iBoneIndex < 0)
        return E_FAIL;

    auto& iRefCount = m_PartialBoneCountMap[iBoneIndex];
    ++iRefCount;

    return S_OK;
}

HRESULT CModel::ReleaseCount_PartialBone(const _char* pBoneName)
{
    if (!pBoneName)
        return E_FAIL;

    _int iBoneIndex = Get_BoneIndex(pBoneName);
    if (iBoneIndex < 0)
        return E_FAIL;

    auto iter = m_PartialBoneCountMap.find(iBoneIndex);

    if (iter != m_PartialBoneCountMap.end())
    {
        --(*iter).second;
    }

    if ((*iter).second <= 0)
        m_PartialBoneCountMap.erase(iter);

    return S_OK;
}

HRESULT CModel::Mapping_OffsetMatrix()
{
    _uint iNumBones = (_uint)m_Bones.size();
    if (iNumBones == 0)
        return S_OK;

    // 1) 전역 Offset 배열 초기화
    m_GlobalOffsetMatrices.clear();
    m_GlobalOffsetMatrices.resize(iNumBones);

    // 1-1) 기본값 (단위행렬)로 세팅
    for (_uint i = 0; i < iNumBones; ++i)
        XMStoreFloat4x4(&m_GlobalOffsetMatrices[i], XMMatrixIdentity());

    // 2) Mesh들을 돌면서 전역 매핑
    for (auto& pMesh : m_Meshes)
    {
        const auto& meshOffsets = pMesh->Get_OffsetMatrices();   // 로컬 Offset
        const auto& meshBoneIndices = pMesh->Get_BoneIndices();      // 로컬 -> 전역 BoneIndex

        _uint localCount = (_uint)meshBoneIndices.size();

        for (_uint local = 0; local < localCount; ++local)
        {
            _int global = meshBoneIndices[local];  // 전역 BoneIndex

            if (global < 0 || global >= (_int)iNumBones)
                continue;

            // 전역 인덱스 위치에 저장
            m_GlobalOffsetMatrices[global] = meshOffsets[local];
        }
    }

    return S_OK;
}

HRESULT CModel::Initialize_Prototype(MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, CModel* pSkeletonModel)
{
    memset(m_szBindTags, 0, sizeof(m_szBindTags));

    _uint         iFlag = {};

    iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    if (MODEL_TYPE::NONANIM == eType)
        iFlag |= aiProcess_PreTransformVertices;


    char szEXT[MAX_PATH] = {};
    char szBinModelFilePath[MAX_PATH] = {};
    _splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);
    if (false == strcmp(".fbx", szEXT))
    {
        if (FAILED(m_pGameInstance->ReadFbx(pModelFilePath, eType, &m_pModel)))
            return E_FAIL;

        char szDrive[MAX_PATH] = {};
        char szDir[MAX_PATH] = {};
        char szFileName[MAX_PATH] = {};
        char szBinExtractor[MAX_PATH] = { ".binx" };
        _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
        strcat_s(szBinModelFilePath, szDrive);
        strcat_s(szBinModelFilePath, szDir);
        strcat_s(szBinModelFilePath, szFileName);
        strcat_s(szBinModelFilePath, szBinExtractor);

        m_pGameInstance->WriteBinx(szBinModelFilePath, eType, &m_pModel);

        //m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
        //if (nullptr == m_pAIScene)
        //   return E_FAIL;



    }
    else if (false == strcmp(".glb", szEXT))
    {
        if (FAILED(m_pGameInstance->ReadFbx(pModelFilePath, eType, &m_pModel)))
            return E_FAIL;

        char szDrive[MAX_PATH] = {};
        char szDir[MAX_PATH] = {};
        char szFileName[MAX_PATH] = {};
        char szBinExtractor[MAX_PATH] = { ".binx" };
        _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
        strcat_s(szBinModelFilePath, szDrive);
        strcat_s(szBinModelFilePath, szDir);
        strcat_s(szBinModelFilePath, szFileName);
        strcat_s(szBinModelFilePath, szBinExtractor);

        m_pGameInstance->WriteBinx(szBinModelFilePath, eType, &m_pModel);

        //m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
        //if (nullptr == m_pAIScene)
        //   return E_FAIL;



    }
    else if (false == strcmp(".bin", szEXT))
    {
        strcpy_s(szBinModelFilePath, pModelFilePath);
        if (FAILED(m_pGameInstance->ReadBin(pModelFilePath, eType, &m_pModel)))
            return E_FAIL;
    }
    else if (false == strcmp(".binx", szEXT))
    {
        strcpy_s(szBinModelFilePath, pModelFilePath);
        if (FAILED(m_pGameInstance->ReadBinx(pModelFilePath, eType, &m_pModel)))
            return E_FAIL;
    }
    else
        return E_FAIL;

    m_eType = eType;
    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);


    // Face, Hair 등 Part Model들의 BlendWeight와 BlendIndex를 Body Model에 매핑해주는 함수.
    if (nullptr != pSkeletonModel)
    {
        Ready_SkeletonBones(pSkeletonModel);
    }
    else
    {
        Ready_Bones(&m_pModel->vNodes[m_pModel->iRootNodeIndex], -1);
    }

    if (FAILED(Ready_Meshes()))
        return E_FAIL;

    if (FAILED(Ready_Materials(pModelFilePath)))
        return E_FAIL;

    if (FAILED(Ready_Animations()))
        return E_FAIL;

    if (FAILED(Mapping_OffsetMatrix()))
        return E_FAIL;

#ifdef _DEBUG
    strcpy_s(m_ModelFilePath, szBinModelFilePath);
#endif

    if (m_eType == MODEL_TYPE::ANIM)
    {
        Initialize_AnimationIndexMap();

        Initialize_AnimationBufferResource();
    }

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    if (m_eType == MODEL_TYPE::ANIM)
    {
        if (FAILED(Ready_ComputeShader()))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(_uint iMeshIndex, CShader* pShader, const _char* pConstantName)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(m_Bones, pShader, pConstantName);

}

HRESULT CModel::Bind_BoneSRV(_uint iMeshIndex, CShader* pShader, const _char* pConstantName)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    if (FAILED(Bind_BoneMatrixSRV(pShader, pConstantName)))
        return E_FAIL;

    if (FAILED(Bind_PreBoneMatrixSRV(pShader)))
        return E_FAIL;

    if (FAILED(Bind_GlobalOffsetMatrices(pShader)))
        return E_FAIL;

    return S_OK;
}

HRESULT CModel::Bind_Material(_uint iMeshIndex, CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    _uint      iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    if (iMaterialIndex >= m_iNumMaterials)
        return E_FAIL;

    return m_Materials[iMaterialIndex]->Bind_SRV(pShader, pConstantName, eType, iTextureIndex);
}

HRESULT CModel::Bind_AllMaterials(_uint iMeshIndex, CShader* pShader, _uint iTextureIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    _uint      iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    if (iMaterialIndex >= m_iNumMaterials)
        return E_FAIL;

    for (_int eType = ENUM_CLASS(TEXTURE_TYPE::NONE); eType < ENUM_CLASS(TEXTURE_TYPE::END); ++eType)
    {
        if (m_szBindTags[eType][0] != '\0')
            m_Materials[iMaterialIndex]->Bind_SRV(pShader, m_szBindTags[eType], Convert_TextureType((TEXTURE_TYPE)eType), iTextureIndex);
    }

    return S_OK;
}

_bool CModel::Play_Animation(_float fTimeDelta, CTransform* pTransform, _float fRootMotionMagnification)
{
    _float fScaledDeltaTime = fTimeDelta * m_fAnimationPlayRate;

    if (-1 == m_iCurrentAnimIndex ||
        m_iCurrentAnimIndex >= m_iNumAnimations)
        return false;

    // 애니메이션 트랙 업데이트
    _int iAnimationState =
        m_Animations[m_iCurrentAnimIndex]->Update_TrackPosition(m_Bones, m_isLoop, fScaledDeltaTime, m_fEndTrackPosition);

    if (iAnimationState == ANIMATIONFLAG_FINISH)
        m_isFinish = TRUE;
    else if (iAnimationState == ANIMATIONFLAG_PLAY)
        m_isFinish = FALSE;
    else if (iAnimationState == ANIMATIONFLAG_RESET)
    {
        if (AnimationChanged)
            AnimationChanged(m_Animations[m_iCurrentAnimIndex]->Get_Name());
        m_isFinish = FALSE;
    }

    m_Animations[m_iCurrentAnimIndex]->Update_CurrentKeyFrameIndices();

    if (m_isLerp && m_fBlendElapsed == 0.f)
    {
        m_pContext->CopyResource(m_pLerpBoneMatrices, m_pOutSource);
        m_pComputeShaderCom->Update_BufferResource(CComputeShader::BUFFER_TYPE::INPUT, 4, m_pLerpBoneMatrices);
    }
    m_pContext->CopyResource(m_pPreBoneMatrices, m_pOutSource);

    if (m_isLerp)
    {
        m_fBlendElapsed += fScaledDeltaTime;

        if (m_fBlendElapsed >= m_fBlendDuration)
        {
            m_fBlendElapsed = m_fBlendDuration;
            m_isLerp = FALSE;
        }

        m_fBlendRatio = m_fBlendElapsed / m_fBlendDuration;
    }
    else
    {
        m_fBlendRatio = 0.f;
    }

    Bind_ComputeShader(fScaledDeltaTime);

    const _uint iNumBones = (_uint)m_Bones.size();

    if (m_pOutReadBack && !m_PartialBoneCountMap.empty())
    {
        m_pContext->CopyResource(m_pOutReadBack, m_pOutSource);

        D3D11_MAPPED_SUBRESOURCE MappedSubResouce{};
        if (SUCCEEDED(m_pContext->Map(m_pOutReadBack, 0, D3D11_MAP_READ, 0, &MappedSubResouce)))
        {
            COMPUTE_BONEMATRIX_OUT* pOut = 
                reinterpret_cast<COMPUTE_BONEMATRIX_OUT*>(MappedSubResouce.pData);

            for (auto& BoneCountIndex : m_PartialBoneCountMap)
            {
                _int iBoneIndex = BoneCountIndex.first;

                m_Bones[iBoneIndex]->Set_TransformationMatrix(
                    XMLoadFloat4x4(&pOut[iBoneIndex].BoneLocalTransformMatrix));

                m_Bones[iBoneIndex]->Set_CombinedTransformationMatrix(
                    XMLoadFloat4x4(&pOut[iBoneIndex].BoneCombinedTransformMatrix));
            }

            m_pContext->Unmap(m_pOutReadBack, 0);
        }
    }

    // 루트모션 적용
    if (pTransform && fRootMotionMagnification != 0.f && m_pOutRootReadBack)
    {
        m_pContext->CopyResource(m_pOutRootReadBack, m_pRootSource);
        Apply_RootMotion(pTransform, fRootMotionMagnification);
    }

    return m_isFinish;
}


HRESULT CModel::Bind_MaterialTag(TEXTURE_TYPE eType, const _char* szBindTag)
{
    if (eType == TEXTURE_TYPE::END)
        return E_FAIL;

    if (eType == TEXTURE_TYPE::NONE)
        return E_FAIL;

    strcpy_s(m_szBindTags[ENUM_CLASS(eType)], szBindTag);
    return S_OK;
}

_bool CModel::CompareAnimationTag(const _char* szAnimationTag)
{
    return m_Animations[m_iCurrentAnimIndex]->CompareAnimationTag(szAnimationTag);
}

aiTextureType CModel::Convert_TextureType(TEXTURE_TYPE eType)
{
    switch (eType)
    {
    case TEXTURE_TYPE::NONE:
        return aiTextureType_NONE;
    case TEXTURE_TYPE::DIFFUSE:
        return aiTextureType_DIFFUSE;
    case TEXTURE_TYPE::SPECULAR:
        return aiTextureType_SPECULAR;
    case TEXTURE_TYPE::AMBIENT:
        return aiTextureType_AMBIENT;
    case TEXTURE_TYPE::EMISSIVE:
        return aiTextureType_EMISSIVE;
    case TEXTURE_TYPE::ORM:
        return aiTextureType_METALNESS;
    case TEXTURE_TYPE::NORMAL:
        return aiTextureType_NORMALS;
    case TEXTURE_TYPE::ORSS:
        return aiTextureType_CLEARCOAT;
    case TEXTURE_TYPE::MASK:
        return aiTextureType_DIFFUSE_ROUGHNESS;
    case TEXTURE_TYPE::EXTRA1:
        return aiTextureType_HEIGHT;
    case TEXTURE_TYPE::EXTRA2:
        return aiTextureType_SHININESS;
    case TEXTURE_TYPE::OPACITY:
        return aiTextureType_OPACITY;
    case TEXTURE_TYPE::EXTRA3:
        return aiTextureType_DISPLACEMENT;
    case TEXTURE_TYPE::EXTRA4:
        return aiTextureType_LIGHTMAP;
    case TEXTURE_TYPE::EXTRA5:
        return aiTextureType_REFLECTION;
    case TEXTURE_TYPE::EXTRA6:
        return aiTextureType_AMBIENT_OCCLUSION;
    case TEXTURE_TYPE::END:
        return aiTextureType_NONE;
    default:
        return aiTextureType_NONE;
    }

    return aiTextureType_NONE;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
    m_Meshes[iMeshIndex]->Bind_Resources();
    m_Meshes[iMeshIndex]->Render();

    return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
    m_iNumMeshes = m_pModel->iNumMeshes;

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, this, &m_pModel->vMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
        if (nullptr == pMesh)
            return E_FAIL;

        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
    /*  텍스쳐를 로드한다 .*/
    m_iNumMaterials = m_pModel->iNumMaterials;

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, pModelFilePath, &m_pModel->vMaterials[i]);
        if (nullptr == pMaterial)
            return E_FAIL;

        m_Materials.push_back(pMaterial);
    }

    return S_OK;
}

HRESULT CModel::Ready_Bones(binNode* pNode, _int iParentIndex)
{
    CBone* pBone = CBone::Create(pNode, iParentIndex);
    if (nullptr == pBone)
        return E_FAIL;

    m_Bones.push_back(pBone);
    // 부모 본에서 자식 본으로, 전체 본의 개수에서 -1을 하며 계속 객체를 생성한다.
    _int iParent = m_Bones.size() - 1;

    for (size_t i = 0; i < pNode->iNumChildren; ++i)
    {
        Ready_Bones(&m_pModel->vNodes[pNode->vChildrenIndex[i]], iParent);
    }

    return S_OK;
}

HRESULT CModel::Ready_Animations()
{
    m_iNumAnimations = m_pModel->iNumAnimations;

    for (size_t i = 0; i < m_iNumAnimations; i++)
    {
        CAnimation* pAnimation = CAnimation::Create(this, &m_pModel->vAnimations[i]);
        if (nullptr == pAnimation)
            return E_FAIL;

        Mapping_Animation(pAnimation);

        m_Animations.push_back(pAnimation);
    }

    return S_OK;
}

HRESULT CModel::Ready_ComputeShader()
{
    _uint iMaxNumKeyFrames = 0;

    for (auto& pAnim : m_Animations)
    {
        auto pChannels = pAnim->Get_vChannels();
        if (pChannels == nullptr)
            continue;

        _uint iTotal = 0;
        for (auto& pChannel : *pChannels)
            iTotal += pChannel->Get_NumKeyFrames();

        if (iTotal > iMaxNumKeyFrames)
            iMaxNumKeyFrames = iTotal;
    }

    // Bone 개수
    _uint iNumData = max((_uint)m_Bones.size(), iMaxNumKeyFrames);
    if (iNumData == 0)
        iNumData = 1;

    m_pComputeShaderCom = CComputeShader::Create(
        m_pDevice, m_pContext,
        TEXT("../Bin/ShaderFiles/Shader_Compute_PlayAnimation.hlsl"),
        "LocalMatrices",
        iNumData);

    m_pCombinedMatrixComputeShaderCom = CComputeShader::Create(
        m_pDevice, m_pContext,
        TEXT("../Bin/ShaderFiles/Shader_Compute_CombinedMatrices.hlsl"),
        "CombinedMatrices",
        iNumData);


    if (nullptr == m_pComputeShaderCom)
        return E_FAIL;

    if (nullptr == m_pCombinedMatrixComputeShaderCom)
        return E_FAIL;

    D3D11_BUFFER_DESC PreBoneBufferDesc = {};
    PreBoneBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    PreBoneBufferDesc.ByteWidth = sizeof(COMPUTE_BONEMATRIX_OUT) * iNumData;
    PreBoneBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    PreBoneBufferDesc.CPUAccessFlags = 0;
    PreBoneBufferDesc.StructureByteStride = sizeof(COMPUTE_BONEMATRIX_OUT);
    PreBoneBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    m_pDevice->CreateBuffer(&PreBoneBufferDesc, nullptr, &m_pPreBoneMatrices);

    m_pDevice->CreateBuffer(&PreBoneBufferDesc, nullptr, &m_pLerpBoneMatrices);

#pragma region GLOBAL BUFFER SETTING

    // 기존의 코드로 비유하면, 전역 변수들을 세팅해주고 셰이더에 바인딩 해주는 과정이다.
    ID3D11Buffer* pBuffer = nullptr;

    m_GlobalBuffer.g_bIsLoop = TRUE;
    m_GlobalBuffer.g_fCurrentTrackPosition = 0.f;
    m_GlobalBuffer.g_fDuration = 0.f;
    m_GlobalBuffer.g_fTickPerSecond = 30.f;
    m_GlobalBuffer.g_fTimeDelta = 0.f;
    m_GlobalBuffer.g_iNumBones = m_Bones.size();
    m_GlobalBuffer.g_iNumChannels = 0;
    m_GlobalBuffer.g_iRootIndex = 0;
    m_GlobalBuffer.g_fBlendRatio = 0.f;
    m_GlobalBuffer._padding = { 0.f, 0.f, 0.f };
    XMStoreFloat4x4(&m_GlobalBuffer.g_PreTransformMatrix, XMMatrixIdentity());

    D3D11_BUFFER_DESC BufferDesc = {};
    BufferDesc.ByteWidth = ((sizeof(COMPUTE_GLOBALBUFFER) + 15) / 16 * 16);
    BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA ConstBufferSubResource = {};
    ConstBufferSubResource.pSysMem = &m_GlobalBuffer;

    if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &ConstBufferSubResource, &pBuffer)))
        return E_FAIL;
    m_pComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::CONSTATNT, pBuffer);

    Safe_AddRef(pBuffer);

    m_pCombinedMatrixComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::CONSTATNT, pBuffer);
#pragma endregion


#pragma region STRUCTURE SETTING
    // 기존의 코드로 비유하면 VS_IN, VS_OUT을 세팅해주는거다.

    D3D11_BUFFER_DESC TrialInitBufferDesc = {};

    D3D11_SUBRESOURCE_DATA SubResource = {};
    // 우리는 기존 컴쉐 친구들과 다르게 구조체를 3개 던져줘야한다.
    // 첫 번째로, Bone을 넣어주자
    {
        TrialInitBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        TrialInitBufferDesc.ByteWidth = sizeof(COMPUTE_BONEINFO) * iNumData;
        TrialInitBufferDesc.StructureByteStride = sizeof(COMPUTE_BONEINFO);
        TrialInitBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        TrialInitBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

        vector<COMPUTE_BONEINFO> vBoneInfos(iNumData);
        for (_uint i = 0; i < iNumData; ++i)
        {
            if (i >= m_Bones.size())
            {
                vBoneInfos[i].iParentIndex = -1;
                vBoneInfos[i]._padding = { 0,0,0 };
            }
            else
            {
                vBoneInfos[i].iParentIndex = m_Bones[i]->Get_ParentBoneIndex();
                vBoneInfos[i]._padding = { 0,0,0 };
            }

        }

        SubResource.pSysMem = vBoneInfos.data();

        if (FAILED(m_pDevice->CreateBuffer(&TrialInitBufferDesc, &SubResource, &pBuffer)))
            return E_FAIL;

        if (FAILED(m_pComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::INPUT, pBuffer)))
            return E_FAIL;

        Safe_AddRef(pBuffer);

        if (FAILED(m_pCombinedMatrixComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::INPUT, pBuffer)))
            return E_FAIL;

        // 이후에 매 프레임마다 바인딩해줘야하므로, 이 버퍼를 들고있어줘야한다 ㅇㅇ. 
        D3D11_BUFFER_DESC ReadBufferDesc = {};
        ReadBufferDesc.Usage = D3D11_USAGE_STAGING;
        ReadBufferDesc.ByteWidth = sizeof(COMPUTE_BONEINFO) * iNumData;
        ReadBufferDesc.StructureByteStride = sizeof(COMPUTE_BONEINFO);
        ReadBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

        if (FAILED(m_pDevice->CreateBuffer(&ReadBufferDesc, nullptr, &m_pBoneSource)))
            return E_FAIL;
    }
    // 두 번째로, Channel을 넣어주자
    {
        TrialInitBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        TrialInitBufferDesc.ByteWidth = sizeof(COMPUTE_CHANNELINFO) * iNumData;
        TrialInitBufferDesc.StructureByteStride = sizeof(COMPUTE_CHANNELINFO);
        TrialInitBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        TrialInitBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    
        vector<COMPUTE_CHANNELINFO> vChannelInfos(iNumData);
        for (_uint i = 0; i < iNumData; ++i)
        {
            vChannelInfos[i].iBoneIndex = 0;
            vChannelInfos[i].iCurrentKeyFrameIndex = 0;
            vChannelInfos[i].iNumKeyFrames = 0;
            vChannelInfos[i].iKeyFrameOffset = 0;
        }
    
        SubResource.pSysMem = vChannelInfos.data();
    
        if (FAILED(m_pDevice->CreateBuffer(&TrialInitBufferDesc, &SubResource, &pBuffer)))
            return E_FAIL;
    
        if (FAILED(m_pComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::INPUT, pBuffer)))
            return E_FAIL;
    
        //// 이후에 매 프레임마다 바인딩해줘야하므로, 이 버퍼를 들고있어줘야한다 ㅇㅇ. 
        //D3D11_BUFFER_DESC ReadBufferDesc = {};
        //ReadBufferDesc.Usage = D3D11_USAGE_STAGING;
        //ReadBufferDesc.ByteWidth = sizeof(COMPUTE_CHANNELINFO) * iNumData;
        //ReadBufferDesc.StructureByteStride = sizeof(COMPUTE_CHANNELINFO);
        //ReadBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
        //
        //if (FAILED(m_pDevice->CreateBuffer(&ReadBufferDesc, nullptr, &m_pChannelSource)))
        //    return E_FAIL;
    }
    // 세 번째로, 키프레임을 넣어주자
    {
        TrialInitBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        TrialInitBufferDesc.ByteWidth = sizeof(COMPUTE_KEYFRAMEINFO) * iNumData;
        TrialInitBufferDesc.StructureByteStride = sizeof(COMPUTE_KEYFRAMEINFO);
        TrialInitBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        TrialInitBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    
        vector<COMPUTE_KEYFRAMEINFO> vKeyFrameInfos(iNumData);
        for (_uint i = 0; i < iNumData; ++i)
        {
            vKeyFrameInfos[i].vScale = { 1.f, 1.f, 1.f };
            vKeyFrameInfos[i].padding01 = 0.f;
            vKeyFrameInfos[i].vRotation = { 1.f, 1.f, 1.f, 1.f };
            vKeyFrameInfos[i].vTranslation = { 1.f, 1.f, 1.f };
            vKeyFrameInfos[i].fTrackPosition = 0.f;
        }
    
        SubResource.pSysMem = vKeyFrameInfos.data();
    
        if (FAILED(m_pDevice->CreateBuffer(&TrialInitBufferDesc, &SubResource, &pBuffer)))
            return E_FAIL;
    
        if (FAILED(m_pComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::INPUT, pBuffer)))
            return E_FAIL;
    
        //// 이후에 매 프레임마다 바인딩해줘야하므로, 이 버퍼를 들고있어줘야한다 ㅇㅇ. 
        //D3D11_BUFFER_DESC ReadBufferDesc = {};
        //ReadBufferDesc.Usage = D3D11_USAGE_STAGING;
        //ReadBufferDesc.ByteWidth = sizeof(COMPUTE_KEYFRAMEINFO) * iNumData;
        //ReadBufferDesc.StructureByteStride = sizeof(COMPUTE_KEYFRAMEINFO);
        //ReadBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
        //
        //if (FAILED(m_pDevice->CreateBuffer(&ReadBufferDesc, nullptr, &m_pKeyFrameSource)))
        //    return E_FAIL;
    }
    // 끝난줄 알았지? 본 초기화를 위한 로컬 본 행렬도 넣어주자
    {
        _uint iNumBones = (_uint)m_Bones.size();
        if (iNumBones > 0)
        {
            vector<COMPUTE_BONEMATRIX_OUT> vInit(iNumData);

            for (_uint i = 0; i < iNumData; ++i)
            {
                if (i < iNumBones)
                {
                    XMStoreFloat4x4(&vInit[i].BoneLocalTransformMatrix,
                        m_Bones[i]->Get_TransformationMatrix());
                    XMStoreFloat4x4(&vInit[i].BoneCombinedTransformMatrix,
                        m_Bones[i]->Get_CombinedTransformationMatrix());
                }
                else
                {
                    XMStoreFloat4x4(&vInit[i].BoneLocalTransformMatrix, XMMatrixIdentity());
                    XMStoreFloat4x4(&vInit[i].BoneCombinedTransformMatrix, XMMatrixIdentity());
                }
            }

            D3D11_BUFFER_DESC FirstDesc{};
            FirstDesc.Usage = D3D11_USAGE_DEFAULT;
            FirstDesc.ByteWidth = sizeof(COMPUTE_BONEMATRIX_OUT) * iNumData;
            FirstDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            FirstDesc.CPUAccessFlags = 0;
            FirstDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
            FirstDesc.StructureByteStride = sizeof(COMPUTE_BONEMATRIX_OUT);

            D3D11_SUBRESOURCE_DATA FirstSubResource{};
            FirstSubResource.pSysMem = vInit.data();

            ID3D11Buffer* pBuffer = nullptr;
            if (FAILED(m_pDevice->CreateBuffer(&FirstDesc, &FirstSubResource, &pBuffer)))
                return E_FAIL;

            ID3D11Buffer* pLerpBuffer = nullptr;
            if (FAILED(m_pDevice->CreateBuffer(&FirstDesc, &FirstSubResource, &pLerpBuffer)))
                return E_FAIL;

            // CComputeShader 쪽 INPUT 버퍼 목록에 추가
            if (FAILED(m_pComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::INPUT, pBuffer)))
            {
                Safe_Release(pBuffer);
                return E_FAIL;
            }

            // CComputeShader 쪽 INPUT 버퍼 목록에 추가
            if (FAILED(m_pComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::INPUT, pLerpBuffer)))
            {
                Safe_Release(pLerpBuffer);
                return E_FAIL;
            }

            D3D11_BUFFER_DESC SecondDesc{};
            SecondDesc.Usage = D3D11_USAGE_DEFAULT;
            SecondDesc.ByteWidth = sizeof(COMPUTE_BONEMATRIX_OUT) * iNumData;
            SecondDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            SecondDesc.CPUAccessFlags = 0;
            SecondDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
            SecondDesc.StructureByteStride = sizeof(COMPUTE_BONEMATRIX_OUT);

            D3D11_SUBRESOURCE_DATA SecondSubResource{};
            SecondSubResource.pSysMem = vInit.data();

            ID3D11Buffer* pSecondBuffer = nullptr;
            if (FAILED(m_pDevice->CreateBuffer(&SecondDesc, &SecondSubResource, &pSecondBuffer)))
                return E_FAIL;

            // CComputeShader 쪽 INPUT 버퍼 목록에 추가
            if (FAILED(m_pCombinedMatrixComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::INPUT, pSecondBuffer)))
            {
                Safe_Release(pBuffer);
                return E_FAIL;
            }


            // 끝난줄 알았지? Out도 세팅해주자
            {
                TrialInitBufferDesc.Usage = D3D11_USAGE_DEFAULT;
                TrialInitBufferDesc.ByteWidth = sizeof(COMPUTE_BONEMATRIX_OUT) * iNumData;
                TrialInitBufferDesc.StructureByteStride = sizeof(COMPUTE_BONEMATRIX_OUT);
                TrialInitBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
                TrialInitBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

                D3D11_SUBRESOURCE_DATA outSub{};
                outSub.pSysMem = vInit.data();

                if (FAILED(m_pDevice->CreateBuffer(&TrialInitBufferDesc, &outSub, &m_pOutSource)))
                    return E_FAIL;

                if (FAILED(m_pComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::OUTPUT, m_pOutSource)))
                    return E_FAIL;

                Safe_AddRef(m_pOutSource);

                if (FAILED(m_pCombinedMatrixComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::OUTPUT, m_pOutSource)))
                    return E_FAIL;
            }

            // 루트모션용 m_pRootSource도 세팅해줘야된다고라고라고라고라고
            {
                TrialInitBufferDesc.Usage = D3D11_USAGE_DEFAULT;
                TrialInitBufferDesc.ByteWidth = sizeof(COMPUTE_BONEMATRIX_OUT);
                TrialInitBufferDesc.StructureByteStride = sizeof(COMPUTE_BONEMATRIX_OUT);
                TrialInitBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
                TrialInitBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

                vector<COMPUTE_BONEMATRIX_OUT> vRootInit(1);

                XMStoreFloat4x4(&vRootInit[0].BoneLocalTransformMatrix, XMMatrixIdentity());
                XMStoreFloat4x4(&vRootInit[0].BoneCombinedTransformMatrix, XMMatrixIdentity());


                D3D11_SUBRESOURCE_DATA RootSubResource{};
                RootSubResource.pSysMem = vRootInit.data();

                Safe_Release(m_pRootSource);
                if (FAILED(m_pDevice->CreateBuffer(&TrialInitBufferDesc, &RootSubResource, &m_pRootSource)))
                    return E_FAIL;

                if (FAILED(m_pCombinedMatrixComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::OUTPUT, m_pRootSource, 1)))
                    return E_FAIL;
            }

        }

    }



    D3D11_BUFFER_DESC readbackDesc = {};
    readbackDesc.Usage = D3D11_USAGE_STAGING;
    readbackDesc.ByteWidth = sizeof(COMPUTE_BONEMATRIX_OUT) * iNumData;
    readbackDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    readbackDesc.StructureByteStride = sizeof(COMPUTE_BONEMATRIX_OUT);
    readbackDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    m_pDevice->CreateBuffer(&readbackDesc, nullptr, &m_pOutReadBack);


    D3D11_BUFFER_DESC RootReadbackDesc = {};
    RootReadbackDesc.Usage = D3D11_USAGE_STAGING;
    RootReadbackDesc.ByteWidth = sizeof(COMPUTE_BONEMATRIX_OUT);
    RootReadbackDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    RootReadbackDesc.StructureByteStride = sizeof(COMPUTE_BONEMATRIX_OUT);
    RootReadbackDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    m_pDevice->CreateBuffer(&RootReadbackDesc, nullptr, &m_pOutRootReadBack);



    // 본들 위치를 갱신해주면서, 루트본을 찾는다.
    for (_int i = 0; i < (_int)m_Bones.size(); ++i)
    {
        if (m_Bones[i]->Compare_Name("Root") || m_Bones[i]->Compare_Name("root"))
        {
            m_iRootIndex = i;
            break;
        }
    }


#pragma endregion

    return S_OK;
}

HRESULT CModel::Ready_SkeletonBones(CModel* pSkeleton)
{
    m_Bones.clear();
    m_Bones.reserve(pSkeleton->Get_Bones()->size());

    for (auto& pSkeletonBone : *pSkeleton->Get_Bones())
    {
        m_Bones.push_back(pSkeletonBone->Clone());
    }

    return S_OK;
}

/// <애니메이션 매핑>
/// 
/// 1. m_Bones의 순서대로 unordered_map에 이름들과 인덱스를 매핑해준다.
/// 2. 이렇게 나온 맵에 따라 기존 채널들을 순서에 맞춰 재배열 해준다.
/// 3. 채널이 없는 m_Bones를 위해 빈 껍데기 채널을 만들어줌
/// 4. 애니메이션에 매핑된 배열을 갖고있게 한다.
///      
/// </애니메이션 매핑>

HRESULT CModel::Mapping_Animation(CAnimation* pAnimation)
{
    if (pAnimation == nullptr)
        return E_FAIL;

    auto pChannels = pAnimation->Get_vChannels();
    if (pChannels == nullptr)
        return E_FAIL;

    _uint iNumBones = (_uint)m_Bones.size();

    // BoneIndex 매핑 테이블 (BoneName → BoneIndex)
    unordered_map<string, _uint> BoneIndexMap;
    BoneIndexMap.reserve(iNumBones);

    for (_uint i = 0; i < iNumBones; ++i)
    {
        BoneIndexMap[m_Bones[i]->Get_Name()] = i;
    }

    // BoneIndex → ChannelIndex 매핑 리스트
    vector<_int> BoneToChannel(iNumBones, -1);

    // 채널을 돌면서 자기 이름에 해당하는 본 인덱스를 찾아 매핑
    for (_uint iChannelIndex = 0; iChannelIndex < pChannels->size(); ++iChannelIndex)
    {
        CChannel* pChannel = (*pChannels)[iChannelIndex];
        auto it = BoneIndexMap.find(pChannel->Get_Name());

        if (it == BoneIndexMap.end())
        {
            // 이 채널 이름에 해당하는 본이 없는 경우 → 그냥 무시
            continue;
        }

        _uint iBoneIndex = it->second;

        BoneToChannel[iBoneIndex] = (_int)iChannelIndex;
        pChannel->Set_BoneIndex(iBoneIndex);
    }

    // 애니메이션 안에 매핑 정보 저장
    pAnimation->Set_BoneToChannelMappingLists(BoneToChannel);

    return S_OK;
}


HRESULT CModel::Bind_ComputeShader(_float fTimeDelta)
{
    // COMPUTE_BONEINFO         m_BoneInfo;
    // COMPUTE_BONEINFO         m_ChannelInfo;
    // COMPUTE_BONEINFO         m_KeyFrameInfo;
    // COMPUTE_BONEINFO         m_GlobalBuffer;

    if (nullptr == m_pComputeShaderCom)
        return E_FAIL;


    m_GlobalBuffer.g_PreTransformMatrix = m_PreTransformMatrix;
    m_GlobalBuffer.g_fTimeDelta = fTimeDelta;
    m_GlobalBuffer.g_fTickPerSecond = m_Animations[m_iCurrentAnimIndex]->Get_TickPerSecond();
    m_GlobalBuffer.g_fCurrentTrackPosition = m_Animations[m_iCurrentAnimIndex]->Get_fTrackPosition();
    m_GlobalBuffer.g_fDuration = m_Animations[m_iCurrentAnimIndex]->Get_Duration();
    m_GlobalBuffer.g_iRootIndex = m_iRootIndex;


    m_GlobalBuffer.g_bIsLoop = m_isLoop;
    m_GlobalBuffer.g_iNumBones = m_Bones.size();
    m_GlobalBuffer.g_iNumChannels = m_Animations[m_iCurrentAnimIndex]->Get_vChannels()->size();

    m_GlobalBuffer.g_fBlendRatio = m_fBlendRatio;

    // 버퍼 세팅?
    // 컴퓨트 셰이더 실행 전에 최종으로 데이터 전달해주는 순간
    // 상수 버퍼 먼저 바로 GPU에 올려준다.
    m_pComputeShaderCom->Update_BufferResource(CComputeShader::BUFFER_TYPE::CONSTATNT, 0, &m_GlobalBuffer);

    {
        _uint iIndex = 0; // 상수버퍼(전역변수)
        m_pComputeShaderCom->Bind_ConstBuffer(1, &iIndex);
    }


    {
        _uint iInputIndices[5] = { 0, 1, 2, 3, 4 }; // Bone, Channel, KeyFrame, InitialLocalMatrix, LerpMatrix
        m_pComputeShaderCom->Bind_InputBuffer(5, iInputIndices);
    }

    {
        _uint iInputIndices[1] = { 0 };
        m_pComputeShaderCom->Bind_OutputBuffer(1, iInputIndices);
    }
    
    _uint iGroupCount = (m_Bones.size() + 127) / 128;

    m_pComputeShaderCom->Update_Shader({ (_float)iGroupCount, 1, 1 });

    // 데이터 가져오는거
    // GetBufferResource
    // 매개변수 1 : 어떤 버퍼 타입에서 데이터를 가져올지
    // 매개변수 2 : 타입에 맞는 버퍼가 몇번째 버퍼인지
    // 매개변수 3 : 값을 받아올 ID3D11Buffer 타입의 변수
    //if(nullptr == m_pOutSource)
    //    m_pComputeShaderCom->GetBufferResource(CComputeShader::BUFFER_TYPE::OUTPUT, 0, m_pOutSource);


    m_pCombinedMatrixComputeShaderCom->Update_BufferResource(CComputeShader::BUFFER_TYPE::CONSTATNT, 0, &m_GlobalBuffer);
    {
        _uint iIndex = 0; // 상수버퍼(전역변수)
        m_pCombinedMatrixComputeShaderCom->Bind_ConstBuffer(1, &iIndex);
    }


    m_pCombinedMatrixComputeShaderCom->Update_BufferResource(CComputeShader::BUFFER_TYPE::INPUT, 1, m_pOutSource);
    {
        _uint iInputIndices[2] = { 0, 1}; // Local,LerpMatrix, 
        m_pCombinedMatrixComputeShaderCom->Bind_InputBuffer(2, iInputIndices);
    }

    {
        _uint iInputIndices[2] = { 0, 1 };
        m_pCombinedMatrixComputeShaderCom->Bind_OutputBuffer(2, iInputIndices);
    }

    m_pCombinedMatrixComputeShaderCom->Update_Shader({ (_float)iGroupCount, 1, 1 });

    // 데이터 가져오는거
    // GetBufferResource
    // 매개변수 1 : 어떤 버퍼 타입에서 데이터를 가져올지
    // 매개변수 2 : 타입에 맞는 버퍼가 몇번째 버퍼인지
    // 매개변수 3 : 값을 받아올 ID3D11Buffer 타입의 변수
    //if (nullptr == m_pOutSource)
    //    m_pCombinedMatrixComputeShaderCom->GetBufferResource(CComputeShader::BUFFER_TYPE::OUTPUT, 0, m_pOutSource);
    //if(nullptr == m_pRootSource)
    //    m_pCombinedMatrixComputeShaderCom->GetBufferResource(CComputeShader::BUFFER_TYPE::OUTPUT, 1, m_pRootSource);

    {
        ID3D11UnorderedAccessView* pNullUAV[2] = { nullptr, nullptr };
        UINT initialCounts[2] = { 0, 1 };

        // CComputeShader에서 UAV를 어떤 슬롯에 물렸는지에 따라 숫자 조정
        // 대부분 0번 슬롯일 가능성이 매우 높음
        m_pContext->CSSetUnorderedAccessViews(0, 2, pNullUAV, initialCounts);
    }



    return S_OK;
}

HRESULT CModel::Apply_RootMotion(CTransform* pTransform, _float fRootMotionMagnification)
{
    if (ROOTFLAG_RESET == m_iFlagPreRootModified)
    {
        m_iFlagPreRootModified = ROOTFLAG_INIT;
        return S_OK;
    }

    if (nullptr == m_pOutRootReadBack)
        return S_OK;

    // 먼저, CBone에 Set도 해줘야하기 때문에 받아와준다.
    D3D11_MAPPED_SUBRESOURCE MappedSubResource{};
    if (SUCCEEDED(m_pContext->Map(m_pOutRootReadBack, 0, D3D11_MAP_READ, 0, &MappedSubResource)))
    {
        COMPUTE_BONEMATRIX_OUT* pOut = reinterpret_cast<COMPUTE_BONEMATRIX_OUT*>(MappedSubResource.pData);

        // 찾았을 때
        if (m_iRootIndex >= 0 && m_iRootIndex < (_int)m_Bones.size())
        {
            m_Bones[m_iRootIndex]->Set_TransformationMatrix(XMLoadFloat4x4(&pOut[0].BoneLocalTransformMatrix));
            m_Bones[m_iRootIndex]->Set_CombinedTransformationMatrix(XMLoadFloat4x4(&pOut[0].BoneCombinedTransformMatrix));

            // 첫 프레임이라는 뜻
            if (ROOTFLAG_INIT == m_iFlagPreRootModified)
            {
                m_PreRootMatrix = pOut[0].BoneCombinedTransformMatrix;
                m_CurRootMatrix = pOut[0].BoneCombinedTransformMatrix;
                m_iFlagPreRootModified = ROOTFLAG_ACTIVE;
            }
            else
            {
                // 루트 이동량이 있을 때, 로컬 본 기준으로 CombinedMatrix를 변화를 준다. 가 지금까지인데, 
                m_PreRootMatrix = m_CurRootMatrix;
                m_CurRootMatrix = pOut[0].BoneCombinedTransformMatrix;

                if ((pTransform != nullptr)
                    && (fRootMotionMagnification != 0.f))
                {
                    _vector vRootAmount = XMVectorSet(
                        m_CurRootMatrix._41 - m_PreRootMatrix._41,
                        m_CurRootMatrix._42 - m_PreRootMatrix._42,
                        m_CurRootMatrix._43 - m_PreRootMatrix._43
                        , 0.f);

                    _matrix matTransform = XMLoadFloat4x4(pTransform->Get_WorldMatrixPtr());
                    matTransform.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

                    vRootAmount = XMVector3TransformNormal(vRootAmount, matTransform);

                    vRootAmount = XMVectorScale(vRootAmount, fRootMotionMagnification);

                    _vector vRootMotion = pTransform->Get_State(STATE::POSITION) + vRootAmount;

                    pTransform->Set_State(STATE::POSITION, vRootMotion);
                }
            }

        }

        m_pContext->Unmap(m_pOutRootReadBack, 0);
    }

    return S_OK;
}

HRESULT CModel::Bind_ChannelAndKeyFrameBuffer()
{
    m_pComputeShaderCom->Update_BufferResource(CComputeShader::BUFFER_TYPE::INPUT, 1, m_pChannelBufferList[m_iCurrentAnimIndex]);

    m_pComputeShaderCom->Update_BufferResource(CComputeShader::BUFFER_TYPE::INPUT, 2, m_pKeyFrameBufferList[m_iCurrentAnimIndex]);
        
    return S_OK;
}

HRESULT CModel::Update_BoneMatrices()
{
    m_pContext->CopyResource(m_pOutReadBack, m_pOutSource);

    return S_OK;
}

_int CModel::Find_Animation(const _char* szAnimationTag)
{
    auto iter = m_AnimationIndexMap.find(szAnimationTag);

    if (iter == m_AnimationIndexMap.end())
        return -1;

    return (*iter).second;
}

HRESULT CModel::Bind_BoneMatrixSRV(CShader* pShader, const _char* pConstantName)
{

    if (m_pBoneMatricesSRV == nullptr)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        SRVDesc.Buffer.NumElements = m_Bones.size();
        HRESULT hr = m_pDevice->CreateShaderResourceView(m_pOutSource, &SRVDesc, &m_pBoneMatricesSRV);
        if (FAILED(hr))
            return hr;
    }

    ID3D11ShaderResourceView* srvs[1] = { m_pBoneMatricesSRV };

    HRESULT hr = pShader->Bind_SRVs(pConstantName, srvs, 1);

    return hr;
}

HRESULT CModel::Bind_PreBoneMatrixSRV(CShader* pShader)
{
    if (m_pPreBoneMatricesSRV == nullptr)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        SRVDesc.Buffer.NumElements = m_Bones.size();
        HRESULT hr = m_pDevice->CreateShaderResourceView(m_pPreBoneMatrices, &SRVDesc, &m_pPreBoneMatricesSRV);
        if (FAILED(hr))
            return hr;
    }
    ID3D11ShaderResourceView* preSRVs[1] = { m_pPreBoneMatricesSRV };

    return pShader->Bind_SRVs("g_PreBoneMatrixBuffer", preSRVs, 1);
}

HRESULT CModel::Bind_GlobalOffsetMatrices(CShader* pShader)
{
    if (!m_GlobalOffsetMatrices.empty())
    {
        _uint iNumOffsets = (_uint)m_GlobalOffsetMatrices.size();
        //512
        if (iNumOffsets > 512)
            iNumOffsets = 512;

        return pShader->Bind_Matrices("g_OffsetMatrices",
            m_GlobalOffsetMatrices.data(),
            iNumOffsets);
    }

    return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, CModel* pSkeletonModel)
{
    CModel* pInstance = new CModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix, pSkeletonModel)))
    {
        MSG_BOX("Failed to Created : CModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
    CModel* pInstance = new CModel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CModel::Free()
{
    __super::Free();

    if (m_isCloned == FALSE)
        Safe_Delete(m_pModel);

    for (auto& pChannelBuffer : m_pChannelBufferList)
        Safe_Release(pChannelBuffer);
    m_pChannelBufferList.clear();

    for (auto& pKeyFrameBuffer : m_pKeyFrameBufferList)
        Safe_Release(pKeyFrameBuffer);
    m_pKeyFrameBufferList.clear();

     
    for (auto& pAnimation : m_Animations)
        Safe_Release(pAnimation);
    m_Animations.clear();

    for (auto& pBone : m_Bones)
        Safe_Release(pBone);
    m_Bones.clear();

    for (auto& pMaterial : m_Materials)
        Safe_Release(pMaterial);
    m_Materials.clear();

    for (auto& pMesh : m_Meshes)
        Safe_Release(pMesh);
    m_Meshes.clear();

    m_GlobalOffsetMatrices.clear();

    Safe_Release(m_pBoneSource);
    Safe_Release(m_pOutReadBack);
    Safe_Release(m_pOutRootReadBack);
    //Safe_Release(m_pOutSource);
    //Safe_Release(m_pRootSource);
    Safe_Release(m_pPreBoneMatrices);
    Safe_Release(m_pLerpBoneMatrices);

    Safe_Release(m_pBoneMatricesSRV);
    Safe_Release(m_pPreBoneMatricesSRV);
    Safe_Release(m_pLerpBoneMatricesSRV);
    Safe_Release(m_pComputeShaderCom);
    Safe_Release(m_pCombinedMatrixComputeShaderCom);

}
