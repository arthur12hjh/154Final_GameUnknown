#include "pch.h"
#include "Player_LockonWalkState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_LockonWalkState::CPlayer_LockonWalkState()
	: CPlayerState {}
{
}

void CPlayer_LockonWalkState::Start(void* pArg)
{
	m_eState = PLAYER_STATE::WALK;

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
	{
		m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Forward"), true, 1.f, 0.3f);
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
	{
		m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Backward"), true, 1.f, 0.3f);
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
	{
		m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Left"), true, 1.f, 0.3f);
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
	{
		m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Right"), true, 1.f, 0.3f);
	}
}

PLAYER_TRANSITION_DESC CPlayer_LockonWalkState::Update(_float fTimeDelta)
{
	_bool isAnimationFinished = m_pPlayer->Play_Animation(fTimeDelta);

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
	{
		m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Forward"), true, 1.f);
		m_Desc->pPlayerTransform->Go_Straight(fTimeDelta);

		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
			m_tNextState.eNextState = PLAYER_STATE::EVADE;
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
	{
		m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Backward"), true, 1.f);
		m_Desc->pPlayerTransform->Go_Backward(fTimeDelta);

		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
			m_tNextState.eNextState = PLAYER_STATE::EVADE;
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
	{
		m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Left"), true, 1.f);
		m_Desc->pPlayerTransform->Go_Left(fTimeDelta);

		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
			m_tNextState.eNextState = PLAYER_STATE::EVADE;
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
	{
		m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Right"), true, 1.f);
		m_Desc->pPlayerTransform->Go_Right(fTimeDelta);

		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
			m_tNextState.eNextState = PLAYER_STATE::EVADE;
	}
	else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
		m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;

	else
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	return m_tNextState;
}

void CPlayer_LockonWalkState::End()
{
}

CPlayer_LockonWalkState* CPlayer_LockonWalkState::Create(void* pArg)
{
	return new CPlayer_LockonWalkState();
}

void CPlayer_LockonWalkState::Free()
{
	__super::Free();
}
