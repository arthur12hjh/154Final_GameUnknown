#include "pch.h"
#include "Player_LandingState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_LandingState::CPlayer_LandingState()
{
}

void CPlayer_LandingState::Start(void* pArg)
{
	m_eState = PLAYER_STATE::LANDING;

	m_pPlayer->Set_Animation("Proto_Jump_End", false, 1.2f);
	m_eType = JUMP_TYPE::IDLE;
}

PLAYER_TRANSITION_DESC CPlayer_LandingState::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	if ((m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)) &&
		fAnimationRatio >= 0.1f)
		m_tNextState.eNextState = PLAYER_STATE::WALK;
	else if (true == isAnimFinished)
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	return m_tNextState;
}

void CPlayer_LandingState::End()
{
}

CPlayer_LandingState* CPlayer_LandingState::Create(void* pArg)
{
	return new CPlayer_LandingState();
}

void CPlayer_LandingState::Free()
{
	__super::Free();
}
