#include "pch.h"

#include "PlayerFSM.h"
#include "Player_IdleState.h"

CPlayerFSM::CPlayerFSM()
{
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
}
