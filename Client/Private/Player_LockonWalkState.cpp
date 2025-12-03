#include "pch.h"
#include "Player_LockonWalkState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_LockonWalkState::CPlayer_LockonWalkState(_bool isRunning)
	: CPlayerState{}
	, m_isRunning { isRunning }
{
}

void CPlayer_LockonWalkState::Start(void* pArg)
{
	m_eState = PLAYER_STATE::WALK;

	if (false == m_isRunning)
	{
		m_isRunStart = true;

		if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Forward_Start"), false, 1.f, 0.15f);
		else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Backward_Start"), false, 1.f, 0.15f);
		else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Left_Start"), false, 1.f, 0.15f);
		else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Right_Start"), false, 1.f, 0.15f);
	}

	else if (true == m_isRunning)
	{
		m_isRunStart = false;

		if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Forward"), false, 1.f, 0.15f);
		else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Backward"), false, 1.f, 0.15f);
		else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Left"), false, 1.f, 0.15f);
		else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Right"), false, 1.f, 0.15f);
	}
}

PLAYER_TRANSITION_DESC CPlayer_LockonWalkState::Update(_float fTimeDelta)
{

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
	{
		if (false == m_isRunStart)
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Forward"), true, 1.f, 0.12f);
		else
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Forward_Start"), false, 1.f, 0.12f);

		m_Desc->pPlayerTransform->Go_Straight(fTimeDelta);

		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
			m_tNextState.eNextState = PLAYER_STATE::EVADE;
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
	{
		if (false == m_isRunStart)
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Backward"), true, 1.f, 0.12f);
		else
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Backward_Start"), false, 1.f, 0.12f);

		m_Desc->pPlayerTransform->Go_Backward(fTimeDelta);

		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
			m_tNextState.eNextState = PLAYER_STATE::EVADE;
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
	{
		if (false == m_isRunStart)
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Left"), true, 1.f, 0.12f);
		else
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Left_Start"), false, 1.f, 0.12f);

		m_Desc->pPlayerTransform->Go_Left(fTimeDelta);

		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
			m_tNextState.eNextState = PLAYER_STATE::EVADE;
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
	{
		if (false == m_isRunStart)
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Right"), true, 1.f, 0.2f, FALSE, -1.f, 14.f, TRUE);
		else
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Run_Right_Start"), false, 1.f, 0.12f);

		m_Desc->pPlayerTransform->Go_Right(fTimeDelta);

		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
			m_tNextState.eNextState = PLAYER_STATE::EVADE;
	}
	else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_1))
		m_tNextState.eNextState = PLAYER_STATE::BETA_TRIPLET;
	else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
		m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;

	else
		m_tNextState.eNextState = PLAYER_STATE::IDLE;


	_bool isAnimationFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	if (true == m_isRunStart && 0.833f <= fAnimationRatio)
		m_isRunStart = false;

	return m_tNextState;
}

void CPlayer_LockonWalkState::End()
{
}

CPlayer_LockonWalkState* CPlayer_LockonWalkState::Create(void* pArg)
{
	if(nullptr == pArg)
		return new CPlayer_LockonWalkState();

	_bool* isRunning = static_cast<_bool*>(pArg);

	if(true == *isRunning)
		return new CPlayer_LockonWalkState(*isRunning);

	return new CPlayer_LockonWalkState(*isRunning);
}

void CPlayer_LockonWalkState::Free()
{
	__super::Free();
}