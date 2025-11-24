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

HRESULT CPrototype_Manager::Add_SkeletalPrototype(_uint iLevelIndex, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strPrototypeTag, const _char* pModelFilePath, const string& strSkeletalPath, _fmatrix PreTransformMatrix)
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddatai64_t  fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst64("../Bin/Resources/Models/Character/PC/Eve/Animation/*.binx*", &fd);

	if (handle == -1)
		return S_OK;

	int iResult = 0;

	if (FAILED(m_pGameInstance->Add_Prototype(iLevelIndex, strPrototypeTag,
		CModel::Create(pDevice, pContext, MODEL_TYPE::ANIM, pModelFilePath, PreTransformMatrix))))
		return E_FAIL;

	CModel* pModel = static_cast<CModel*>(Find_Prototype(iLevelIndex, strPrototypeTag));

	while (iResult != -1)
	{
		CBase* pAnimModel = nullptr;

		int iLength = strlen(fd.name) + 1;
		WCHAR* pFileName = new WCHAR[iLength];
		ZeroMemory(pFileName, sizeof(WCHAR) * iLength);
		// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
		MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pFileName, iLength);

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
			CModel::Create(pDevice, pContext, MODEL_TYPE::ANIM, szFullPath.c_str()), &pAnimModel)))
			return E_FAIL;

		if (FAILED(pModel->Import_Animations(static_cast<CModel*>(pAnimModel)->Get_AnimationList())))
			return E_FAIL;

		iResult = _findnext64(handle, &fd);
		Safe_Delete_Array(pFileName);
	}

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
