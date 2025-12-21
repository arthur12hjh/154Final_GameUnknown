#include "pch.h"

#include "Player_BlinkState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_BlinkState::CPlayer_BlinkState()
	: CPlayerState{}
{
}

//시간 느려지면서 이펙트 나와야하는 모션. ( + 림라이트도 )
void CPlayer_BlinkState::Start(void* pArg, _float fBlendRatio)
{
	m_eState = PLAYER_STATE::BLINK;
	m_pPlayer->Set_Animation("P_Eve_Sword_Normal_FlashBehindAttack_S", true, 1.2f);
}

PLAYER_TRANSITION_DESC CPlayer_BlinkState::Update(_float fTimeDelta)
{
	return m_tNextState;
}

_float CPlayer_BlinkState::End()
{
	return m_fNextBlendRatio;
}

CPlayer_BlinkState* CPlayer_BlinkState::Create(void* pArg)
{
	return new CPlayer_BlinkState();
}

void CPlayer_BlinkState::Free()
{
	__super::Free();
}
