#include "pch.h"
#include "Player_BattleLandingState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_BattleLandingState::CPlayer_BattleLandingState()
{
}

void CPlayer_BattleLandingState::Start(void* pArg)
{
	m_eState = PLAYER_STATE::LANDING;

	m_pPlayer->Set_Animation("Proto_Battle_Jump_End", false, 1.2f);
	m_eType = JUMP_TYPE::IDLE;
}

PLAYER_TRANSITION_DESC CPlayer_BattleLandingState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	if ((m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)) &&
		fAnimationRatio >= 0.75f)
		m_tNextState.eNextState = PLAYER_STATE::WALK;
	else if (true == isAnimFinished)
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	return m_tNextState;
}

void CPlayer_BattleLandingState::End()
{
}

CPlayer_BattleLandingState* CPlayer_BattleLandingState::Create(void* pArg)
{
	return new CPlayer_BattleLandingState();
}

void CPlayer_BattleLandingState::Free()
{
	__super::Free();
}
