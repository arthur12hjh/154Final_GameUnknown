#include "Prototype_Manager.h"
#include "GameObject.h"
#include "Component.h"

#include "GameInstance.h"

#include "Model.h"

CPrototype_Manager::CPrototype_Manager()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPrototype_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	m_pPrototypes = new map<const _wstring, CBase*>[iNumLevels];

	return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype, CBase** ppOut)
{
	if (iLevelIndex >= m_iNumLevels || 
		nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
		return E_FAIL;	

	m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);

	if (ppOut)
		*ppOut = pPrototype;

	return S_OK;
}

/*
 - 뼈대 모델에 애니메이션과, 파츠 모델을 붙이는 프로토타입 함수
*/
HRESULT CPrototype_Manager::Add_SkeletalPrototype(_uint iLevelIndex, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strPrototypeTag, const _char* pModelFilePath, const string& strSkeletalPath, vector<_wstring>& szPartPrototypeTagList, vector<string>& szPartModelFilePathList, _fmatrix PreTransformMatrix)
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddatai64_t  fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	// 이것도 경로, 확장자명 유연하게 바꿔야하는데 일단 이 함수 쓸 객체가 플레이어 밖에 없으니까 냅둔다.
	_char szPath[MAX_PATH];
	strcpy_s(szPath, strSkeletalPath.c_str());
	const _char szEXT[MAX_PATH] = "*.binx*";
	
	strcat_s(szPath, szEXT);
	intptr_t handle = _findfirst64(szPath, &fd);

	if (handle == -1)
		return S_OK;

	int iResult = 0;

	// 먼저 뼈대 모델. 즉 기반이 되는 모델을 만든다.
	if (FAILED(m_pGameInstance->Add_Prototype(iLevelIndex, strPrototypeTag, 
		CModel::Create(pDevice, pContext, MODEL_TYPE::ANIM, pModelFilePath, PreTransformMatrix))))
		return E_FAIL;

	auto pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CModel* pModel = static_cast<CModel*>(pPrototype);

	// 뼈대 모델에 애니메이션을 매핑해준다.
	while (iResult != -1)
	{
		CBase* pAnimModel = nullptr;

		int iLength = strlen(fd.name) + 1;
		WCHAR* pFileName = new WCHAR[iLength];
		ZeroMemory(pFileName, sizeof(WCHAR) * iLength);
		// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
		MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pFileName, iLength);

		_char		szAnimationTag[MAX_PATH] = {};
		_splitpath_s(fd.name, nullptr, 0, nullptr, 0, szAnimationTag, MAX_PATH, nullptr, 0);

		string szFullPath = strSkeletalPath + fd.name;
		string szFullTag = "Prototype_Component_Model_";
		szFullTag += fd.name;

		WCHAR* szPrototypeTemporaryTag = nullptr;

		int iPrototypeLength = MultiByteToWideChar(CP_UTF8, 0, szFullTag.c_str(), -1, NULL, 0);
		if (iPrototypeLength <= 0)
			return E_FAIL;

		wstring szPrototypeTag(iPrototypeLength, 0);
		MultiByteToWideChar(CP_UTF8, 0, szFullTag.c_str(), -1, &szPrototypeTag[0], iPrototypeLength);

		/* For.Prototype_Component_Model_Eve_Anim01_P_Eve_Sword_Normal_LinkAttack1_Scarlet */
		if (FAILED(Add_Prototype(iLevelIndex, szPrototypeTag,
			CModel::Create(pDevice, pContext, MODEL_TYPE::ANIMONLY, szFullPath.c_str()), &pAnimModel)))
			return E_FAIL;

		if (FAILED(pModel->Import_Animations(static_cast<CModel*>(pAnimModel)->Get_AnimationList(), szAnimationTag)))
			return E_FAIL;

		auto iter = m_pPrototypes[iLevelIndex].find(szPrototypeTag);

		if (iter != m_pPrototypes[iLevelIndex].end())
		{
			m_pPrototypes[iLevelIndex].erase(iter);
			Safe_Release(pAnimModel);
		}

		iResult = _findnext64(handle, &fd);
		Safe_Delete_Array(pFileName);
	}

	_matrix PreMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	// 파츠 모델들을 생성하고, 블렌드인덱스와 본 인덱스를 뼈대 모델에 맞춰 매핑해준다.
	for (size_t i = 0; i < szPartPrototypeTagList.size(); ++i)
	{
		// 모델에 스켈레톤 모델을 전달해줘 매핑할 수 있게 해준다.
		if (FAILED(m_pGameInstance->Add_Prototype(iLevelIndex, szPartPrototypeTagList[i],
			CModel::Create(pDevice, pContext, MODEL_TYPE::PARTANIM, szPartModelFilePathList[i].c_str(), PreMatrix, pModel))))
			return E_FAIL;

	}

	pModel->Initialize_AnimationIndexMap();
	pModel->Initialize_AnimationBufferResource();
	pModel->Release_AnimationChannel();

	_findclose(handle);

	return S_OK;
}

CBase* CPrototype_Manager::Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	CBase* pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CBase* pGameObject = { nullptr };

	if (PROTOTYPE::GAMEOBJECT == ePrototype)
		pGameObject = dynamic_cast<CGameObject*>(pPrototype)->Clone(pArg);
	else
		pGameObject = dynamic_cast<CComponent*>(pPrototype)->Clone(pArg);

	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

const map<const _wstring, class CBase*>* CPrototype_Manager::Get_Prototypes_InLevel(_uint iLevelIndex) 
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;
	return &m_pPrototypes[iLevelIndex];
}

void CPrototype_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pPrototypes[iLevelIndex])	
		Safe_Release(Pair.second);

	m_pPrototypes[iLevelIndex].clear();
}

CBase* CPrototype_Manager::Get_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag)
{
	return Find_Prototype(iLevelIndex, strPrototypeTag);
}

_bool CPrototype_Manager::bIsClearLevelResource(_uint iLevelID)
{
	if (0 > iLevelID || m_iNumLevels <= iLevelID)
	{
		MSG_BOX("Out Bound Level ID");
		return false;
	}

	if (m_pPrototypes[iLevelID].empty())
		return true;

	return false;
}

CBase* CPrototype_Manager::Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;		

	auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);

	if(iter == m_pPrototypes[iLevelIndex].end())	
		return nullptr;

	return iter->second;
}

CPrototype_Manager* CPrototype_Manager::Create(_uint iNumLevels)
{
	CPrototype_Manager* pInstance = new CPrototype_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CPrototype_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPrototype_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pPrototypes[i])		
			Safe_Release(Pair.second);
		m_pPrototypes[i].clear();		
	}
	Safe_Delete_Array(m_pPrototypes);


	Safe_Release(m_pGameInstance);
}
