#include "Object_Manager.h"
#include "GameInstance.h"

#include "Layer.h"
#include "GameObject.h"

CObject_Manager::CObject_Manager()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CComponent* CObject_Manager::Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(strComponentTag, iIndex);	
}

CComponent* CObject_Manager::Get_PartObject_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPartTag, const _wstring& strComponentTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_PartObject_Component(strPartTag, strComponentTag, iIndex);	
}

list<CGameObject *>* CObject_Manager::Get_LayerObjects(_uint iLevelIndex, const WCHAR* szLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, szLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->GetObjects();
}

map<const _wstring, class CLayer*>* CObject_Manager::GetLayer()
{
	_uint LevelID = m_pGameInstance->GetCurrentLevelID();

	return &m_pLayers[LevelID];
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	m_pLayers = new map<const _wstring, CLayer*>[iNumLevels];

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject_ToLayer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	CGameObject*	pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer*		pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);	

	return S_OK;
}

CGameObject* CObject_Manager::Add_Get_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pGameObject)
		return nullptr;

	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return pGameObject;
}

void CObject_Manager::ADD_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CGameObject* pObject)
{
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pObject);
		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pObject);
}

void CObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Priority_Update(fTimeDelta);
	}
}

void CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Update(fTimeDelta);
	}
}

void CObject_Manager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Late_Update(fTimeDelta);
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
		Pair.second->Clear();

	m_iPreLevel = iLevelIndex;
}

void CObject_Manager::Clear_DeadObj()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Clear_DeadObj();

			if (-1 != m_iPreLevel && m_iPreLevel == i)
				Safe_Release(Pair.second);
		}
	}

	if (-1 != m_iPreLevel)
	{
		m_pLayers[m_iPreLevel].clear();
		m_iPreLevel = -1;
	}
}


CLayer* CObject_Manager::Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	if (iLayerLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pLayers[iLayerLevelIndex].find(strLayerTag);
	if(iter == m_pLayers[iLayerLevelIndex].end())		
		return nullptr;

	return iter->second;
}

CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CObject_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);
}
