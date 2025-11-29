#include "pch.h"

#include "PlayerFSM.h"
#include "Player_IdleState.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

CPlayerFSM::CPlayerFSM() 
	: m_pGameInstance { CGameInstance::GetInstance() }
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGameManager);

	//참조 용도
	m_pPlayer = CGameManager::GetInstance()->GetGameCharacter();
	Safe_Release(m_pPlayer);
}

PLAYER_STATE CPlayerFSM::Get_StateEnum()
{
	return m_pCurrentState->Get_State(); 
}

void CPlayerFSM::Clear_FSM()
{
	Safe_Release(m_pCurrentState);
}

void CPlayerFSM::Free()
{
	__super::Free();
	
	Safe_Release(m_pCurrentState);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pGameManager);
}
