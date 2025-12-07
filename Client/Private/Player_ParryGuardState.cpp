#include "pch.h"
#include "Player_ParryGuardState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_ParryGuardState::CPlayer_ParryGuardState()
    : CPlayerState{}
{
}

void CPlayer_ParryGuardState::Start(void* pArg, _float fBlendRatio)
{
    m_eState = PLAYER_STATE::PARRY_GUARD;
    m_pPlayer->Set_Animation("Proto_Hit_Guard", false, 1.5f, 0.12f);

	m_Desc->isLookFixed = true;
}

PLAYER_TRANSITION_DESC CPlayer_ParryGuardState::Update(_float fTimeDelta)
{
	_bool isAnimationFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	if (0.3f <= fAnimationRatio)
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
	{
		PLAYER_PARRY_DESC Desc;
		Desc.isImmediate = true;

		m_tNextState.eNextState = PLAYER_STATE::PARRY;
		m_tNextState.pArg = &Desc;
	}

	return m_tNextState;
}

_float CPlayer_ParryGuardState::End()
{
	m_Desc->isLookFixed = false;

    return m_fNextBlendRatio;
}

CPlayer_ParryGuardState* CPlayer_ParryGuardState::Create(void* pArg)
{
    return new CPlayer_ParryGuardState();
}

void CPlayer_ParryGuardState::Free()
{
	__super::Free();
}
