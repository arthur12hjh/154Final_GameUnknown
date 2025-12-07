#include "pch.h"
#include "Player_ParrySuccessState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_ParrySuccessState::CPlayer_ParrySuccessState()
    : CPlayerState{}
{
}

void CPlayer_ParrySuccessState::Start(void* pArg, _float fBlendRatio)
{
	m_Desc->isLookFixed = true;
    m_eState = PLAYER_STATE::PARRY_SUCCESS;
    m_pPlayer->Set_Animation("Proto_Guard_Parry", false, 2.f, 0.12f);
	m_pGameInstance->Active_RadialBlur(0.5f, 8, 0.2f);
}

PLAYER_TRANSITION_DESC CPlayer_ParrySuccessState::Update(_float fTimeDelta)
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

	//else if (0.6f <= fAnimationRatio && true == m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E))
	//{
	//	m_tNextState.eNextState = PLAYER_STATE::PARRY;
	//}
	if(0.27f <= fAnimationRatio && m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E))
	{
		PLAYER_PARRY_DESC Desc;
		Desc.isImmediate = true;
		m_tNextState.eNextState = PLAYER_STATE::PARRY;
		m_fNextBlendRatio = 0.4f;
		m_tNextState.pArg = &Desc;
	}
	else if (0.31f <= fAnimationRatio)
	{
		m_tNextState.eNextState = PLAYER_STATE::PARRY_END;
		m_fNextBlendRatio = 0.12f;
	}

    return m_tNextState;
}

_float CPlayer_ParrySuccessState::End()
{
	m_Desc->isLookFixed = false;
	return m_fNextBlendRatio;
}

CPlayer_ParrySuccessState* CPlayer_ParrySuccessState::Create(void* pArg)
{
    return new CPlayer_ParrySuccessState();
}

void CPlayer_ParrySuccessState::Free()
{
    __super::Free();
}
