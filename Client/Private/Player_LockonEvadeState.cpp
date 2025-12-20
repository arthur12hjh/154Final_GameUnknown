#include "pch.h"
#include "Player_LockonEvadeState.h"

#include "GameInstance.h"
#include "Player.h"

CPlayer_LockonEvadeState::CPlayer_LockonEvadeState()
	: CPlayerState {}
{
}

void CPlayer_LockonEvadeState::Start(void* pArg, _float fBlendRatio)
{
	m_eState = PLAYER_STATE::EVADE;

	_float fDegree = 0.f;

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
	{
		m_pPlayer->Set_Animation("Proto_Lockon_Evade_Forward", false, 1.2f);
		m_eDirection = EVADE_DIR::STRAIGHT;
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
	{
		m_pPlayer->Set_Animation("Proto_Lockon_Evade_Left", false, 1.2f);
		m_eDirection = EVADE_DIR::LEFT;
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
	{
		m_pPlayer->Set_Animation("Proto_Lockon_Evade_Right", false, 1.2f);
		m_eDirection = EVADE_DIR::RIGHT;
	}
	else
	{
		m_pPlayer->Set_Animation("Proto_Lockon_Evade_Backward", false, 1.2f);
		m_eDirection = EVADE_DIR::BACKWARD;
	}
}

PLAYER_TRANSITION_DESC CPlayer_LockonEvadeState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	//ratio 받아와서 세팅
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();


	switch (m_eDirection)
	{
	case EVADE_DIR::STRAIGHT:
		if (fAnimationRatio <= 0.3f)
			m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 1.6f);
		break;
	case EVADE_DIR::LEFT:
		if (fAnimationRatio <= 0.3f)
			m_Desc->pPlayerTransform->Go_Left(fTimeDelta * 1.6f);
		break;
	case EVADE_DIR::RIGHT:
		if (fAnimationRatio <= 0.3f)
			m_Desc->pPlayerTransform->Go_Right(fTimeDelta * 1.6f);
		break;
	case EVADE_DIR::BACKWARD:
		if (fAnimationRatio <= 0.3f)
			m_Desc->pPlayerTransform->Go_Backward(fTimeDelta * 1.6f);
		break;
	}

	if (fAnimationRatio > 0.4f && (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)))
	{
		m_tNextState.eNextState = PLAYER_STATE::WALK;
		m_tNextStateDesc.isEvade = true;
		m_tNextState.pArg = &m_tNextStateDesc;

		if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
			m_tNextState.eNextState = PLAYER_STATE::SPRINT;

	}
	else if (fAnimationRatio > 0.45f && m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
		m_tNextState.eNextState = PLAYER_STATE::LIGHT_ATTACK;
	else if (fAnimationRatio > 0.45f && m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_E) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E))
		m_tNextState.eNextState = PLAYER_STATE::PARRY;

	else if (fAnimationRatio > 0.45f &&  m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_W) ||
		m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_A) ||
		m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_S) ||
		m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_D))
		m_tNextState.eNextState = PLAYER_STATE::WALK;
	else if (fAnimationRatio > 0.45f &&  m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_1))
		m_tNextState.eNextState = PLAYER_STATE::BETA_TRIPLET;

	else if (fAnimationRatio > 0.45f &&  m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_2))
		m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;

	if (true == isAnimFinished)
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	return m_tNextState;
}

_float CPlayer_LockonEvadeState::End()
{
	return 0.12f;
}

CPlayer_LockonEvadeState* CPlayer_LockonEvadeState::Create(void* pArg)
{
	return new CPlayer_LockonEvadeState();
}

void CPlayer_LockonEvadeState::Free()
{
	__super::Free();
}
