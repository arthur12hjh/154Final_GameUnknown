#include "pch.h"
#include "Player_BattleSprintLandingState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_BattleSprintLandingState::CPlayer_BattleSprintLandingState()
	: CPlayerState{}
{
	m_eState = PLAYER_STATE::LANDING;

	m_pPlayer->Set_Animation("Proto_Battle_Jump_End", false, 1.2f);
	m_eType = JUMP_TYPE::IDLE;
}

void CPlayer_BattleSprintLandingState::Start(void* pArg, _float fBlendRatio)
{
	m_eState = PLAYER_STATE::LANDING;

	m_pPlayer->Set_Animation("Proto_Battle_Jump_End", false, 1.2f);
	m_eType = JUMP_TYPE::IDLE;
}

PLAYER_TRANSITION_DESC CPlayer_BattleSprintLandingState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	if ((m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)) &&
		fAnimationRatio >= 0.75f)
	{
		//걸어
		m_tNextState.eNextState = PLAYER_STATE::WALK;

		//쉬프트도 누르고 있었으면 뛰어
		if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
			m_tNextState.eNextState = PLAYER_STATE::SPRINT;
	}

	else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_E) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E))
		m_tNextState.eNextState = PLAYER_STATE::PARRY;

	if (true == isAnimFinished)
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	return m_tNextState;
}

_float CPlayer_BattleSprintLandingState::End()
{
	return m_fNextBlendRatio;
}

CPlayer_BattleSprintLandingState* CPlayer_BattleSprintLandingState::Create(void* pArg)
{
	return new CPlayer_BattleSprintLandingState();
}

void CPlayer_BattleSprintLandingState::Free()
{
	__super::Free();
}
