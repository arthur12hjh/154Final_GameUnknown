#include "Model.h"

#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Material.h"
#include "Animation.h"
#include "Channel.h"
#include "ComputeShader.h"


#include "GameInstance.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext }
{
}

CModel::CModel(const CModel& Prototype)
    : CComponent{ Prototype }
	, m_eType { Prototype.m_eType }
	, m_iNumMeshes { Prototype.m_iNumMeshes }
	, m_Meshes { Prototype.m_Meshes }
	, m_iNumMaterials{ Prototype.m_iNumMaterials }
	, m_Materials{ Prototype.m_Materials }
	, m_PreTransformMatrix { Prototype.m_PreTransformMatrix }
	, m_iNumAnimations{ Prototype.m_iNumAnimations }
	, m_GlobalOffsetMatrices{ Prototype.m_GlobalOffsetMatrices }
{
	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& pMaterial : m_Materials)
		Safe_AddRef(pMaterial);

	for (auto& pPrototypeAnim : Prototype.m_Animations)
		m_Animations.push_back(pPrototypeAnim->Clone());

	memcpy(m_szBindTags, Prototype.m_szBindTags, sizeof(m_szBindTags));

	m_pBoneMatricesSRV = nullptr;
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
	_int	iBoneIndex = {};

	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool 
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

_uint CModel::Get_AnimationKeyFrameIndex() const
{
	return m_Animations[m_iCurrentAnimIndex]->Get_AnimationKeyFrameIndex();
}

const _float4x4* CModel::Get_BoneMatrixPtr(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
	{
		if (true == pBone->Compare_Name(pBoneName))
			return true;

		return false;
	});


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

void CModel::Set_Animation(const _char* szAnimationTag, _bool isLoop)
{
	_uint iAnimIndex = 0;
	for (auto& pAnimation : m_Animations)
	{
		if (TRUE == pAnimation->CompareAnimationTag(szAnimationTag))
		{
			if (m_iCurrentAnimIndex == iAnimIndex)
				return;

			m_iCurrentAnimIndex = iAnimIndex;
			m_isLoop = isLoop;

			m_Animations[m_iCurrentAnimIndex]->Reset();

			if (AnimationChanged)
				AnimationChanged(m_Animations[m_iCurrentAnimIndex]->Get_Name());
			return;
		}

		++iAnimIndex;
	}
}

HRESULT CModel::Import_Animations(vector<class CAnimation*>* pAnimations)
{
	if (nullptr == pAnimations)
		return E_FAIL;

	for (auto& pAnimation : *pAnimations)
	{
		Mapping_Animation(pAnimation);

		m_Animations.push_back(pAnimation);
		++m_iNumAnimations;
	}


	return S_OK;
}

HRESULT CModel::Import_Texture(_uint iMeshIndex, TEXTURE_TYPE eType, CTexture* pTexture, const _char* pBindTag)
{
	// 1. m_Meshes에 Get_MaterialIndex()를 해서 머티리얼 인덱스를 받아온다.
	// 2. m_Materials에 Import_Texture()로 SRV를 넣어준다.
	// 3. pBindTag가 nullptr가 아니면 m_szBindTags에 복사해준다.
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	if(pTexture == nullptr)
		return E_FAIL;

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (pBindTag != nullptr)
		strcpy_s(m_szBindTags[static_cast<_uint>(eType)], pBindTag);

	return m_Materials[iMaterialIndex]->Import_Texture(Convert_TextureType(eType), pTexture->Get_SRV());
}

HRESULT CModel::Import_Texture(_uint iMeshIndex, TEXTURE_TYPE eType, const _char* pTextureFilePath, const _char* pBindTag)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	char	strTexturePath[MAX_PATH];

	char		szDrive[MAX_PATH] = {};
	char		szDir[MAX_PATH] = {};
	char		szFileName[MAX_PATH] = {};
	char		szEXT[MAX_PATH] = {};

	strcpy_s(strTexturePath, pTextureFilePath);

	_char		szTextureFilePath[MAX_PATH] = {};
	_splitpath_s(pTextureFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
	_splitpath_s(strTexturePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

	strcpy_s(szTextureFilePath, szDrive);
	strcat_s(szTextureFilePath, szDir);
	strcat_s(szTextureFilePath, szFileName);
	strcat_s(szTextureFilePath, szEXT);

	_tchar		szAbsolutePath[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, strlen(szTextureFilePath),
		szAbsolutePath, MAX_PATH);

	ID3D11ShaderResourceView* pSRV = { nullptr };

	HRESULT			hr = {};

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

	return m_Materials[iMaterialIndex]->Import_Texture(Convert_TextureType(eType), pSRV);
}

HRESULT CModel::Import_Texture(_uint iMeshIndex, TEXTURE_TYPE eType, ID3D11ShaderResourceView* pSRV, const _char* pBindTag)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (pBindTag != nullptr)
		strcpy_s(m_szBindTags[static_cast<_uint>(eType)], pBindTag);

	return m_Materials[iMaterialIndex]->Import_Texture(Convert_TextureType(eType), pSRV);
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

HRESULT CModel::Initialize_Prototype(MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	memset(m_szBindTags, 0, sizeof(m_szBindTags));

	_uint			iFlag = {};

	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (MODEL_TYPE::NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices;

	char szEXT[MAX_PATH] = {};
	_splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);
	if (false == strcmp(".fbx", szEXT))
	{
		if (FAILED(m_pGameInstance->ReadFbx(pModelFilePath, eType, &m_pModel)))
			return E_FAIL;

		char szBinModelFilePath[MAX_PATH] = {};
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
		//	return E_FAIL;



	}
	else if (false == strcmp(".glb", szEXT))
	{
		if (FAILED(m_pGameInstance->ReadFbx(pModelFilePath, eType, &m_pModel)))
			return E_FAIL;

		char szBinModelFilePath[MAX_PATH] = {};
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
		//	return E_FAIL;



	}
	else if (false == strcmp(".bin", szEXT))
	{
		if (FAILED(m_pGameInstance->ReadBin(pModelFilePath, eType, &m_pModel)))
			return E_FAIL;
	}
	else if (false == strcmp(".binx", szEXT))
	{
		if (FAILED(m_pGameInstance->ReadBinx(pModelFilePath, eType, &m_pModel)))
			return E_FAIL;
	}
	else
		return E_FAIL;

	m_eType = eType;
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	Ready_Bones(&m_pModel->vNodes[m_pModel->iRootNodeIndex], -1);

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	if (FAILED(Mapping_OffsetMatrix()))
		return E_FAIL;

	Safe_Delete(m_pModel);

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	
	if (m_eType == MODEL_TYPE::NONANIM)
		return S_OK;

	m_pBoneMatricesSRV = nullptr;

	if (FAILED(Ready_ComputeShader()))
		return E_FAIL;


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

	if (m_pBoneMatricesSRV == nullptr) {
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
	if (FAILED(hr))
		OutputDebugString(L"Bind_BoneSRV: Bind_SRVs FAILED\n");
	else
		OutputDebugString(L"Bind_BoneSRV: Bind_SRVs OK\n");

	if (!m_GlobalOffsetMatrices.empty())
	{
		_uint iNumOffsets = (_uint)m_GlobalOffsetMatrices.size();
		//512
		if (iNumOffsets > 512)
			iNumOffsets = 512;

		hr = pShader->Bind_Matrices("g_OffsetMatrices",
			m_GlobalOffsetMatrices.data(),
			iNumOffsets);

		if (FAILED(hr))
		{
			OutputDebugString(L"Bind_BoneSRV: Bind_Matrices(g_OffsetMatrices) FAILED\n");
			return hr;
		}
	}

	return S_OK;
}

HRESULT CModel::Bind_Material(_uint iMeshIndex, CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex]->Bind_SRV(pShader, pConstantName, eType, iTextureIndex);
}

HRESULT CModel::Bind_AllMaterials(_uint iMeshIndex, CShader* pShader, _uint iTextureIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	for (_int eType = ENUM_CLASS(TEXTURE_TYPE::NONE); eType < ENUM_CLASS(TEXTURE_TYPE::END); ++eType)
	{
		if (m_szBindTags[eType][0] != '\0')
			m_Materials[iMaterialIndex]->Bind_SRV(pShader, m_szBindTags[eType], Convert_TextureType((TEXTURE_TYPE)eType), iTextureIndex);
	}

	return S_OK;
}
/*
*  바인딩 필요한 값들
*  - pass 0:
*		*** Global ***
*			-> g_fCurrentTrackPosition
*			-> g_fTimeDelta
*			-> g_fTickPerSecond
*			-> g_fDuration
*			-> g_bIsLoop
*			-> g_iNumBones
*			-> g_iNumChannels
*		[ ] Channel
*			-> iBoneIndex
*			-> iCurrentKeyFrameIndex
*			-> iNumKeyFrames
*			-> iKeyFrameOffset
*		[ ] KeyFrame
*			-> vScale
*			-> vRotation
*			-> vTranslation
*			-> fTrackPosition
* 
*	현재 산재된 문제점
*	-> 채널과 본 수가 다름 개 씨발 매핑 되게 순서대로 정렬은 해줬는데 예외 되는지 확인 필요
*	-> 원래 채널에서 선형보간 시 pCurrentKeyFrameIndex를 해주는데 그건 옮기지 말고 CPU에서 해주어야함
* 
*	바인딩을 어디서 할 것인가?
*	-> 그래도 여기서 해야지..
*/
_bool CModel::Play_Animation(_float fTimeDelta, _bool isSimd)
{
	if (isSimd)
	{
		if (-1 == m_iCurrentAnimIndex ||
			m_iCurrentAnimIndex >= m_iNumAnimations)
			return false;

		/* 내가 재생하고자하는 애니메이션(공격모션)이 이용하고 있는 뼈들의 상태 변환정보(TransformationMatrix)를 갱신해준다.*/
		m_isFinish = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, fTimeDelta);


		/* 모든 뼈를 순회하면서 CombinedTransformationMatrix를 갱신한다. */
		for (auto& pBone : m_Bones)
		{
			pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
		}

		return m_isFinish;
	}
	else
	{
		if (-1 == m_iCurrentAnimIndex ||
			m_iCurrentAnimIndex >= m_iNumAnimations)
			return false;

		/* 내가 재생하고자하는 애니메이션(공격모션)이 이용하고 있는 뼈들의 상태 변환정보(TransformationMatrix)를 갱신해준다.*/
		//m_isFinish = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, fTimeDelta);
		m_isFinish = m_Animations[m_iCurrentAnimIndex]->Update_TrackPosition(m_Bones, m_isLoop, fTimeDelta);

		m_Animations[m_iCurrentAnimIndex]->Update_CurrentKeyFrameIndices();

		Bind_ComputeShader(fTimeDelta);

		return m_isFinish;
	}
	
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
	case TEXTURE_TYPE::MASK:
		return aiTextureType_DIFFUSE_ROUGHNESS;
	case TEXTURE_TYPE::EXTRA1:
		return aiTextureType_HEIGHT;
	case TEXTURE_TYPE::EXTRA2:
		return aiTextureType_SHININESS;
	case TEXTURE_TYPE::EXTRA3:
		return aiTextureType_OPACITY;
	case TEXTURE_TYPE::EXTRA4:
		return aiTextureType_DISPLACEMENT;
	case TEXTURE_TYPE::EXTRA5:
		return aiTextureType_LIGHTMAP;
	case TEXTURE_TYPE::EXTRA6:
		return aiTextureType_REFLECTION;
	case TEXTURE_TYPE::EXTRA7:
		return aiTextureType_AMBIENT_OCCLUSION;
	case TEXTURE_TYPE::EXTRA8:
		return aiTextureType_CLEARCOAT;
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
		"CombinedMatrices",
		iNumData);

	if (nullptr == m_pComputeShaderCom)
		return E_FAIL;

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
	m_GlobalBuffer.g_BatchOffset = 0;
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
			vBoneInfos[i].iParentIndex = -1;
			vBoneInfos[i]._padding = { 0,0,0 };
		}

		SubResource.pSysMem = vBoneInfos.data();

		if (FAILED(m_pDevice->CreateBuffer(&TrialInitBufferDesc, &SubResource, &pBuffer)))
			return E_FAIL;

		if (FAILED(m_pComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::INPUT, pBuffer)))
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

		// 이후에 매 프레임마다 바인딩해줘야하므로, 이 버퍼를 들고있어줘야한다 ㅇㅇ. 
		D3D11_BUFFER_DESC ReadBufferDesc = {};
		ReadBufferDesc.Usage = D3D11_USAGE_STAGING;
		ReadBufferDesc.ByteWidth = sizeof(COMPUTE_CHANNELINFO) * iNumData;
		ReadBufferDesc.StructureByteStride = sizeof(COMPUTE_CHANNELINFO);
		ReadBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

		if (FAILED(m_pDevice->CreateBuffer(&ReadBufferDesc, nullptr, &m_pChannelSource)))
			return E_FAIL;
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

		// 이후에 매 프레임마다 바인딩해줘야하므로, 이 버퍼를 들고있어줘야한다 ㅇㅇ. 
		D3D11_BUFFER_DESC ReadBufferDesc = {};
		ReadBufferDesc.Usage = D3D11_USAGE_STAGING;
		ReadBufferDesc.ByteWidth = sizeof(COMPUTE_KEYFRAMEINFO) * iNumData;
		ReadBufferDesc.StructureByteStride = sizeof(COMPUTE_KEYFRAMEINFO);
		ReadBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

		if (FAILED(m_pDevice->CreateBuffer(&ReadBufferDesc, nullptr, &m_pKeyFrameSource)))
			return E_FAIL;
	}
	// 끝난줄 알았지? 본 초기화를 위한 로컬 본 행렬도 넣어주자
	{
		_uint iNumBones = (_uint)m_Bones.size();
		if (iNumBones > 0)
		{
			std::vector<COMPUTE_BONEMATRIX_OUT> vInit(iNumData);

			for (_uint i = 0; i < iNumData; ++i)
			{
				if(i < iNumBones)
					XMStoreFloat4x4(&vInit[i].BoneLocalTransformMatrix, m_Bones[i]->Get_TransformationMatrix());
				else
					XMStoreFloat4x4(&vInit[i].BoneLocalTransformMatrix, XMMatrixIdentity());

			}

			D3D11_BUFFER_DESC desc{};
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(COMPUTE_BONEMATRIX_OUT) * iNumData;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(COMPUTE_BONEMATRIX_OUT);

			D3D11_SUBRESOURCE_DATA sub{};
			sub.pSysMem = vInit.data();

			ID3D11Buffer* pBuffer = nullptr;
			if (FAILED(m_pDevice->CreateBuffer(&desc, &sub, &pBuffer)))
				return E_FAIL;

			// CComputeShader 쪽 INPUT 버퍼 목록에 추가
			if (FAILED(m_pComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::INPUT, pBuffer)))
			{
				Safe_Release(pBuffer);
				return E_FAIL;
			}


		}

	}

	// 끝난줄 알았지? Out도 세팅해주자
	{
		TrialInitBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		TrialInitBufferDesc.ByteWidth = sizeof(COMPUTE_BONEMATRIX_OUT) * iNumData;
		TrialInitBufferDesc.StructureByteStride = sizeof(COMPUTE_BONEMATRIX_OUT);
		TrialInitBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		TrialInitBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		if (FAILED(m_pDevice->CreateBuffer(&TrialInitBufferDesc, nullptr, &m_pOutSource)))
			return E_FAIL;

		if (FAILED(m_pComputeShaderCom->ADD_Buffer(CComputeShader::BUFFER_TYPE::OUTPUT, m_pOutSource)))
			return E_FAIL;

	}
	
	D3D11_BUFFER_DESC readbackDesc = {};
	readbackDesc.Usage = D3D11_USAGE_STAGING;
	readbackDesc.ByteWidth = sizeof(COMPUTE_BONEMATRIX_OUT) * iNumData;
	readbackDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	readbackDesc.StructureByteStride = sizeof(COMPUTE_BONEMATRIX_OUT);
	readbackDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	m_pDevice->CreateBuffer(&readbackDesc, nullptr, &m_pOutReadBack);

#pragma endregion

	return S_OK;
}

/// <애니메이션 매핑>
/// 
/// 1. m_Bones의 순서대로 unordered_map에 이름들과 인덱스를 매핑해준다.
/// 2. 이렇게 나온 맵에 따라 기존 채널들을 순서에 맞춰 재배열 해준다.
/// 3. 채널이 없는 m_Bones를 위해 빈 껍데기 채널을 만들어줌
/// 4. 애니메이션에 매핑된 배열을 갖고있게 한다.
/// 
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
	// COMPUTE_BONEINFO			m_BoneInfo;
	// COMPUTE_BONEINFO			m_ChannelInfo;
	// COMPUTE_BONEINFO			m_KeyFrameInfo;
	// COMPUTE_BONEINFO			m_GlobalBuffer;

	if (nullptr == m_pComputeShaderCom)
		return E_FAIL;


	m_GlobalBuffer.g_PreTransformMatrix = m_PreTransformMatrix;
	// 고민 1: 이거 근데 여기서 이렇게 하면 Update_TransformationMatrices 호출 안되지 않나? 어떻게 해야하지?
	// Update_TrackPosition라는 함수를 만들어서, 거기서 m_fCurrentTrackPosition을 관리해주면 어떨까?
	//  ㄴ 거기서 CChannel::Update_TransformationMatrix를 대체하는 함수도 만들어보자
	m_GlobalBuffer.g_fTimeDelta = fTimeDelta;
	m_GlobalBuffer.g_fTickPerSecond = m_Animations[m_iCurrentAnimIndex]->Get_TickPerSecond();
	m_GlobalBuffer.g_fCurrentTrackPosition = m_Animations[m_iCurrentAnimIndex]->Get_fTrackPosition();
	m_GlobalBuffer.g_fDuration = m_Animations[m_iCurrentAnimIndex]->Get_Duration();
	m_GlobalBuffer.g_BatchOffset = 0;


	m_GlobalBuffer.g_bIsLoop = m_isLoop;
	m_GlobalBuffer.g_iNumBones = m_Bones.size();
	m_GlobalBuffer.g_iNumChannels = m_Animations[m_iCurrentAnimIndex]->Get_vChannels()->size();

	// 버퍼 세팅?
	// 컴퓨트 셰이더 실행 전에 최종으로 데이터 전달해주는 순간
	// 상수 버퍼 먼저 바로 GPU에 올려준다.
	m_pComputeShaderCom->Update_BufferResource(CComputeShader::BUFFER_TYPE::CONSTATNT, 0, &m_GlobalBuffer);

	_uint iNumData = m_Bones.size();

	// 구조체가 3개라는건.. 귀찮다는 뜻이다.
	// 각자 별개의 방식으로 바인딩 해줘야한다는 뜻이다.
	{
		vector<COMPUTE_BONEINFO> vBoneInfos(iNumData);

		for (_uint i = 0; i < m_Bones.size(); ++i)
		{
			vBoneInfos[i].iParentIndex = m_Bones[i]->Get_ParentBoneIndex();
			vBoneInfos[i]._padding = { 0,0,0 };
		}

		if (m_pBoneSource)
		{
			D3D11_MAPPED_SUBRESOURCE SubResource{};
			if (SUCCEEDED(m_pContext->Map(m_pBoneSource, 0, D3D11_MAP_WRITE, 0, &SubResource)))
			{
				memcpy(SubResource.pData, vBoneInfos.data(), sizeof(COMPUTE_BONEINFO) * iNumData);
				m_pContext->Unmap(m_pBoneSource, 0);
			}

			m_pComputeShaderCom->Update_BufferResource(CComputeShader::BUFFER_TYPE::INPUT, 0, m_pBoneSource);
		}
	}

	// 2) Channel이랑 KeyFrame은, uint iKeyFrameOffset 때문에 같이 바인딩 해주는걸 권한다.
	{
		vector<COMPUTE_CHANNELINFO> vChannelInfos(m_Bones.size());
		vector<COMPUTE_KEYFRAMEINFO> vKeyFrameInfos;

		auto pChannels = m_Animations[m_iCurrentAnimIndex]->Get_vChannels();
		auto& BoneToChannelMappingList = m_Animations[m_iCurrentAnimIndex]->Get_BoneToChannelMappingLists();

		_uint iKeyFrameOffset = 0;

		for (_uint i = 0; i < m_Bones.size(); ++i)
		{
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
				m_Animations[m_iCurrentAnimIndex]->Get_AnimationKeyFrameIndex(iChannelIndex);

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

		// ChannelInfo 업로드
		if (m_pChannelSource)
		{
			D3D11_MAPPED_SUBRESOURCE SubResource{};
			if (SUCCEEDED(m_pContext->Map(m_pChannelSource, 0, D3D11_MAP_WRITE, 0, &SubResource)))
			{
				memcpy(SubResource.pData, vChannelInfos.data(),
					sizeof(COMPUTE_CHANNELINFO) * m_Bones.size());
				m_pContext->Unmap(m_pChannelSource, 0);
			}
			m_pComputeShaderCom->Update_BufferResource(CComputeShader::BUFFER_TYPE::INPUT, 1, m_pChannelSource);
		}

		// KeyFrameInfo 업로드
		if (m_pKeyFrameSource)
		{
			D3D11_MAPPED_SUBRESOURCE SubResource{};
			if (SUCCEEDED(m_pContext->Map(m_pKeyFrameSource, 0, D3D11_MAP_WRITE, 0, &SubResource)))
			{
				if (!vKeyFrameInfos.empty())
				{
					memcpy(SubResource.pData, vKeyFrameInfos.data(),
						sizeof(COMPUTE_KEYFRAMEINFO) * static_cast<size_t>(vKeyFrameInfos.size()));
				}
				m_pContext->Unmap(m_pKeyFrameSource, 0);
			}
			m_pComputeShaderCom->Update_BufferResource(CComputeShader::BUFFER_TYPE::INPUT, 2, m_pKeyFrameSource);
		}
	}


	{
		_uint iIndex = 0; // 상수버퍼(전역변수)
		m_pComputeShaderCom->Bind_ConstBuffer(1, &iIndex);
	}


	{
		_uint iInputIndices[4] = { 0, 1, 2, 3 }; // Bone, Channel, KeyFrame, InitialLocalMatrix
		m_pComputeShaderCom->Bind_InputBuffer(4, iInputIndices);
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
	m_pComputeShaderCom->GetBufferResource(CComputeShader::BUFFER_TYPE::OUTPUT, 0, m_pOutSource);


	//// 1) GPU → CPU 복사용 Staging Buffer로 데이터 복사
	//m_pContext->CopyResource(m_pOutReadBack, m_pOutSource);
	//
	//// 2) STAGING 버퍼를 읽는다
	//D3D11_MAPPED_SUBRESOURCE MappedResource;
	//if (SUCCEEDED(m_pContext->Map(m_pOutReadBack, 0, D3D11_MAP_READ, 0, &MappedResource)))
	//{
	//	COMPUTE_BONEMATRIX_OUT* pOut =
	//		reinterpret_cast<COMPUTE_BONEMATRIX_OUT*>(MappedResource.pData);
	//
	//	for (_uint i = 0; i < m_Bones.size(); i++)
	//	{
	//		m_Bones[i]->Set_TransformationMatrix(
	//			XMLoadFloat4x4(&pOut[i].BoneLocalTransformMatrix)
	//		);
	//		m_Bones[i]->Set_CombinedTransformationMatrix(
	//			XMLoadFloat4x4(&pOut[i].BoneCombinedTransformMatrix)
	//		);
	//	}
	//
	//	m_pContext->Unmap(m_pOutReadBack, 0);
	//}

	
	return S_OK;
}


CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
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
    
	Safe_Release(m_pBoneSource);
	Safe_Release(m_pChannelSource);
	Safe_Release(m_pKeyFrameSource);
	Safe_Release(m_pOutReadBack);

	Safe_Release(m_pComputeShaderCom);


}
