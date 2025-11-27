#include "pch.h"
#include "Player_HitState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_HitState::CPlayer_HitState()
	: CPlayerState {}
{
}

void CPlayer_HitState::Start(void* pArg)
{
	m_eState = PLAYER_STATE::HIT;

	m_pPlayer->Set_Animation("Proto_Jump_End", false);
}

PLAYER_TRANSITION_DESC CPlayer_HitState::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	// 끝나면 Idle 
	if (true == isAnimFinished)
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	// 움직이려고 하면 캔슬해서 걷기
	else if ((m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)) 
		&& fAnimationRatio >= 0.3f)
		m_tNextState.eNextState = PLAYER_STATE::WALK;

	return m_tNextState;
}

void CPlayer_HitState::End()
{
}

CPlayer_HitState* CPlayer_HitState::Create(void* pArg)
{
	return new CPlayer_HitState();
}

void CPlayer_HitState::Free()
{
	__super::Free();
}
