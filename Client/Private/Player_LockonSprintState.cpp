#include "pch.h"
#include "Player_LockonSprintState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_LockonSprintState::CPlayer_LockonSprintState()
{
}

void CPlayer_LockonSprintState::Start(void* pArg, _float fBlendRatio)
{
	m_eState = PLAYER_STATE::SPRINT;

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
	{
		m_pPlayer->Set_Animation("Proto_Lockon_Sprint_Forward_Start", false, 1.2f, 0.15f);
		m_eDirection = PLAYER_DIRECTION::STRAIGHT;
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
	{
		m_pPlayer->Set_Animation("Proto_Lockon_Sprint_Left_Start", false, 1.2f, 0.15f);
		m_eDirection = PLAYER_DIRECTION::LEFT;
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
	{
		m_pPlayer->Set_Animation("Proto_Lockon_Sprint_Right_Start", false, 1.2f, 0.15f);
		m_eDirection = PLAYER_DIRECTION::RIGHT;
	}

    m_isSprintStart = true;
}

PLAYER_TRANSITION_DESC CPlayer_LockonSprintState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) &&
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT) &&
		m_eDirection == PLAYER_DIRECTION::STRAIGHT)
	{
		if (false == m_isSprintStart && false == m_isRunning)
		{
			m_pPlayer->Set_Animation("Proto_Battle_Sprint", true, 1.2f, 0.f);
			m_isRunning = true;
		}
		else if (PLAYER_DIRECTION::STRAIGHT != m_eDirection && false == m_isSprintStart && true == m_isRunning)
			m_pPlayer->Set_Animation("Proto_Battle_Sprint", true, 1.2f, 1.f, false, -1.f, 1.f, true, true);

		m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 2.f);

	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) &&
			 m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT) &&
			 m_eDirection == PLAYER_DIRECTION::LEFT)
	{
		if (false == m_isSprintStart && false == m_isRunning)
		{
			m_pPlayer->Set_Animation("Proto_Battle_Sprint_Left", true, 1.2f, 0.08f, false, -1.f, 0.f);
			m_isRunning = true;
		}
		else if(PLAYER_DIRECTION::LEFT != m_eDirection && false == m_isSprintStart && true == m_isRunning)
			m_pPlayer->Set_Animation("Proto_Battle_Sprint_Left", true, 1.2f, 1.f, false);

		m_Desc->pPlayerTransform->Go_Left(fTimeDelta * 2.f);
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D) &&
			 m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT) &&
			 m_eDirection == PLAYER_DIRECTION::RIGHT)
	{
		if (false == m_isSprintStart && false == m_isRunning)
		{
			m_pPlayer->Set_Animation("Proto_Battle_Sprint_Right", true, 1.2f, 0.f, false, -1.f, 4.f);
			m_isRunning = true;
		}
		else if (PLAYER_DIRECTION::RIGHT != m_eDirection && false == m_isSprintStart && true == m_isRunning)
			m_pPlayer->Set_Animation("Proto_Battle_Sprint_Right", true, 1.2f, 1.f, false);

		m_Desc->pPlayerTransform->Go_Right(fTimeDelta * 2.f);
	}
	else
	{
		m_tNextState.eNextState = PLAYER_STATE::SPRINT_END;
		m_tNextState.pArg = &m_eDirection;
	}


	if (PLAYER_DIRECTION::RIGHT == m_eDirection && true == isAnimFinished && true == m_isSprintStart)
	{
		m_isSprintStart = false;
	}
	else if (PLAYER_DIRECTION::STRAIGHT == m_eDirection && true == isAnimFinished && true == m_isSprintStart)
	{
		m_isSprintStart = false;
	}
	else if (PLAYER_DIRECTION::LEFT == m_eDirection && (28.f/32.f) <= fAnimationRatio && true == m_isSprintStart)
	{
		m_isSprintStart = false;
	}



	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
	{
		PLAYER_JUMP_DESC Desc;
		Desc.isSprintJump = true;

		m_tNextState.eNextState = PLAYER_STATE::JUMP;
		m_tNextState.pArg = &Desc;
	}
	else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
		m_tNextState.eNextState = PLAYER_STATE::EVADE;
	else if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
		m_tNextState.eNextState = PLAYER_STATE::LIGHT_ATTACK;
	else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_E) || m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E))
		m_tNextState.eNextState = PLAYER_STATE::PARRY;
	else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_1))
		m_tNextState.eNextState = PLAYER_STATE::BETA_TRIPLET;
	else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
		m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;

	return m_tNextState;
}

_float CPlayer_LockonSprintState::End()
{
    return m_fNextBlendRatio;
}

CPlayer_LockonSprintState* CPlayer_LockonSprintState::Create(void* pArg)
{
    return new CPlayer_LockonSprintState();
}

void CPlayer_LockonSprintState::Free()
{
	__super::Free();
}