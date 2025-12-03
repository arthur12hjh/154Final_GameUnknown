#include "pch.h"

#include "Player_IdleLandingState.h"

#include "GameInstance.h"
#include "Player.h"

CPlayer_IdleLandingState::CPlayer_IdleLandingState()
    : CPlayerState{}
{
}

void CPlayer_IdleLandingState::Start(void* pArg)
{
    m_eState = PLAYER_STATE::LANDING;
    m_pPlayer->Set_Animation("Proto_Jump_End", false, 1.2f);
}

PLAYER_TRANSITION_DESC CPlayer_IdleLandingState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	if ((m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)) &&
		fAnimationRatio >= 0.5f)
		m_tNextState.eNextState = PLAYER_STATE::WALK;
	else if (true == isAnimFinished)
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	return m_tNextState;
}

void CPlayer_IdleLandingState::End()
{
}

CPlayer_IdleLandingState* CPlayer_IdleLandingState::Create(void* pArg)
{
    return new CPlayer_IdleLandingState();
}

void CPlayer_IdleLandingState::Free()
{
	__super::Free();
}
