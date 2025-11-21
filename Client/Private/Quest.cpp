#include "pch.h"
#include "Quest.h"

#include "GameInstance.h"

CQuest::CQuest() :
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CQuest::Initialize()
{
	return S_OK;
}

void CQuest::Free()
{
	Safe_Release(m_pGameInstance);
}
