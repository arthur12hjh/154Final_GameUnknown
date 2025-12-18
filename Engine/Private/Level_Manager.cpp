#include "Level_Manager.h"
#include "GameInstance.h"

#include "Level.h"

CLevel_Manager::CLevel_Manager()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);	
}

HRESULT CLevel_Manager::Change_Level(CLevel* pNewLevel)
{
	if (nullptr != m_pCurrentLevel)
	{
		Safe_Release(m_pCurrentLevel);
	}
	//넘어가기 직전에 그림자 구워줌.
	m_pGameInstance->Bake_StaticShadow();
	
	m_pCurrentLevel = pNewLevel;
	
	return S_OK;
}

void CLevel_Manager::Update(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;	

	return m_pCurrentLevel->Render();
}

_uint CLevel_Manager::GetCurrentLevelID()
{
	return m_pCurrentLevel->Get_LevelID();
}

CLevel* CLevel_Manager::GetCurrentLevel()
{
	return m_pCurrentLevel;
}

HRESULT CLevel_Manager::Clear_LevelResource(_bool bIsClearProtoTypes)
{
	if (nullptr != m_pCurrentLevel)
	{
		m_pGameInstance->Clear_Resources(m_pCurrentLevel->Get_LevelID(), bIsClearProtoTypes);
	}
		
	return S_OK;
}

CLevel_Manager* CLevel_Manager::Create()
{
	return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pCurrentLevel);
	Safe_Release(m_pGameInstance);
}
