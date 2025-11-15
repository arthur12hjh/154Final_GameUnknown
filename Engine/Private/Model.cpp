#include "Model.h"

#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Material.h"
#include "Animation.h"
#include "Channel.h"

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
	, m_PreTransformMatrix { Prototype.m_PreTransformMatrix}
	, m_iNumAnimations { Prototype.m_iNumAnimations }
{
	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& pMaterial : m_Materials)
		Safe_AddRef(pMaterial);

	for (auto& pPrototypeAnim : Prototype.m_Animations)
		m_Animations.push_back(pPrototypeAnim->Clone());
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
	}

	return S_OK;
}

HRESULT CModel::Initialize_Prototype(MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
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
		char szBinExtractor[MAX_PATH] = { ".bin" };
		_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
		strcat_s(szBinModelFilePath, szDrive);
		strcat_s(szBinModelFilePath, szDir);
		strcat_s(szBinModelFilePath, szFileName);
		strcat_s(szBinModelFilePath, szBinExtractor);
		 
		m_pGameInstance->WriteBin(szBinModelFilePath, eType, &m_pModel);

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
		char szBinExtractor[MAX_PATH] = { ".bin" };
		_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
		strcat_s(szBinModelFilePath, szDrive);
		strcat_s(szBinModelFilePath, szDir);
		strcat_s(szBinModelFilePath, szFileName);
		strcat_s(szBinModelFilePath, szBinExtractor);

		m_pGameInstance->WriteBin(szBinModelFilePath, eType, &m_pModel);

		//m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
		//if (nullptr == m_pAIScene)
		//	return E_FAIL;



	}
	else if (false == strcmp(".bin", szEXT))
	{
		if (FAILED(m_pGameInstance->ReadBin(pModelFilePath, eType, &m_pModel)))
			return E_FAIL;

		char szFbxModelFilePath[MAX_PATH] = {};
		char szDrive[MAX_PATH] = {};
		char szDir[MAX_PATH] = {};
		char szFileName[MAX_PATH] = {};
		char szFbxExtractor[MAX_PATH] = { ".fbx" };
		_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
		strcat_s(szFbxModelFilePath, szDrive);
		strcat_s(szFbxModelFilePath, szDir);
		strcat_s(szFbxModelFilePath, szFileName);
		strcat_s(szFbxModelFilePath, szFbxExtractor);

		//m_pAIScene = m_Importer.ReadFile(szFbxModelFilePath, iFlag);
		//if (nullptr == m_pAIScene)
		//	return E_FAIL;
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

	Safe_Delete(m_pModel);

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(_uint iMeshIndex, CShader* pShader, const _char* pConstantName)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(m_Bones, pShader, pConstantName);
	
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

_bool CModel::Play_Animation(_float fTimeDelta)
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

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

/// <애니메이션 매핑>
/// 
/// 1. 먼저 Bone을 unordered_map으로 만들어서 매핑하기 편하게 한다.
/// 2. map에 이름과 Bone Index를 넣는다.
/// 3. ChannelList를 순회하며 Channel->Get_Name()으로 Bone Index를 찾는다.
/// 4. pAnimation의 해당 인덱스와 기존 채널 Index에 Swap을 한다.
/// 5. Safe_AddRef(pAnimation) 이후 m_Animations에 push_back한다.
///		
/// </애니메이션 매핑>

HRESULT CModel::Mapping_Animation(CAnimation* pAnimation)
{
	/// 1. 먼저 Bone을 unordered_map으로 만들어서 매핑하기 편하게 한다.
	unordered_map<string, _uint> BoneIndexMap;
	BoneIndexMap.reserve(m_Bones.size());
	for (size_t i = 0; i < m_Bones.size(); ++i)
	{
		/// 2. map에 이름과 Bone Index를 넣는다.
		BoneIndexMap[m_Bones[i]->Get_Name()] = i;
	}

	/// 3. ChannelList를 순회하며 Channel->Get_Name()으로 Bone Index를 찾는다.
	_uint iChannelIndex = 0;
	for (auto& pChannel : *pAnimation->Get_vChannels())
	{
		auto it = BoneIndexMap.find(pChannel->Get_Name());

		if (it != BoneIndexMap.end())
		{
			pAnimation->Swap_AnimationChannel(iChannelIndex, it->second);
			pChannel->Set_BoneIndex(Get_BoneIndex(pChannel->Get_Name()));
		}
		else
		{
			
		}

		++iChannelIndex;
	}

	m_Animations.push_back(pAnimation);

	Safe_AddRef(pAnimation);

	++m_iNumAnimations;

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
    
}
