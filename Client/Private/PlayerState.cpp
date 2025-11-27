#include "pch.h"

#include "Player.h"
#include "PlayerState.h"

#include "GameInstance.h"
#include "GameManager.h"

CPlayerState::CPlayerState()
	: m_pGameInstance { CGameInstance::GetInstance() }
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGameManager);

	//순환참조나요..
	m_pPlayer = static_cast<CPlayer*>(m_pGameManager->GetGameCharacter());
	m_Desc = m_pPlayer->Get_Desc();
	Safe_Release(m_pPlayer);
}

void CPlayerState::Start(void* pArg)
{
}

CPlayerState* CPlayerState::Update(_float fTimeDelta)
{
	return nullptr;
}

void CPlayerState::End()
{
}

void CPlayerState::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pGameManager);
}
