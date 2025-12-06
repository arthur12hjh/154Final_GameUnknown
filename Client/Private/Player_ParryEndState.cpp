#include "pch.h"
#include "Player_ParryEndState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_ParryEndState::CPlayer_ParryEndState()
	: CPlayerState{}
{
}

void CPlayer_ParryEndState::Start(void* pArg, _float fBlendRatio)
{
	m_eState = PLAYER_STATE::PARRY_END;
	m_pPlayer->Set_Animation("Proto_Guard_End", false, 1.45f, fBlendRatio);
}

PLAYER_TRANSITION_DESC CPlayer_ParryEndState::Update(_float fTimeDelta)
{
	_bool isAnimationFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	if (0.4f <= fAnimationRatio)
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
			m_tNextState.eNextState = PLAYER_STATE::LIGHT_ATTACK;

		else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
			m_tNextState.eNextState = PLAYER_STATE::EVADE;

		else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_W) ||
			m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_A) ||
			m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_S) ||
			m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_D))
			m_tNextState.eNextState = PLAYER_STATE::WALK;
	}

	if (true == isAnimationFinished)
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	return m_tNextState;
}

_float CPlayer_ParryEndState::End()
{
	return m_fNextBlendRatio;
}

CPlayer_ParryEndState* CPlayer_ParryEndState::Create(void* pArg)
{
	return new CPlayer_ParryEndState();
}

void CPlayer_ParryEndState::Free()
{
	__super::Free();
}
