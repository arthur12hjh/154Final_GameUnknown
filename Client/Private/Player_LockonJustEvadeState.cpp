#include "pch.h"
#include "Player_LockonJustEvadeState.h"

#include "GameInstance.h"
#include "Player.h"

CPlayer_LockonJustEvadeState::CPlayer_LockonJustEvadeState()
	: CPlayerState{}
{
}

void CPlayer_LockonJustEvadeState::Start(void* pArg, _float fBlendRatio)
{
	m_pGameInstance->Active_RadialBlur(0.5f, 16, 0.4f);

	m_eState = PLAYER_STATE::JUST_EVADE;
	m_Desc->isInvincible = true;
	
	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
	{
		m_pPlayer->Set_Animation("Proto_Lockon_Sword_Just_Evade1_Forward_3", false, 1.2f);
		m_eDirection = PLAYER_DIRECTION::STRAIGHT;
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
	{
		m_pPlayer->Set_Animation("Proto_Lockon_Sword_Just_Evade1_Left_3", false, 1.2f);
		m_eDirection = PLAYER_DIRECTION::LEFT;
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
	{
		m_pPlayer->Set_Animation("Proto_Lockon_Sword_Just_Evade1_Right_3", false, 1.2f);
		m_eDirection = PLAYER_DIRECTION::RIGHT;
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
	{
		m_pPlayer->Set_Animation("Proto_Lockon_Sword_Just_Evade1_backward_3", false, 1.2f);
		m_eDirection = PLAYER_DIRECTION::BACKWARD;
	}

	m_pGameInstance->SetGameSpeed(0.7f);
}

PLAYER_TRANSITION_DESC CPlayer_LockonJustEvadeState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	//ratio 받아와서 세팅
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	switch (m_eDirection)
	{
	case PLAYER_DIRECTION::STRAIGHT:
		if (fAnimationRatio <= 0.53f)
			m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 1.8f);
		break;
	case PLAYER_DIRECTION::LEFT:
		if (fAnimationRatio <= 0.53f)
			m_Desc->pPlayerTransform->Go_Left(fTimeDelta * 1.8f);
		break;
	case PLAYER_DIRECTION::RIGHT:
		if (fAnimationRatio <= 0.53f)
			m_Desc->pPlayerTransform->Go_Right(fTimeDelta * 1.8f);
		break;
	case PLAYER_DIRECTION::BACKWARD:
		if (fAnimationRatio <= 0.53f)
			m_Desc->pPlayerTransform->Go_Backward(fTimeDelta * 1.8f);
		break;
	}

	if(0.4 <= fAnimationRatio)
		m_pGameInstance->SetGameSpeed(1.f);

	if (true == isAnimFinished)
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	//키입력을 이용한 캔슬도 필요함.

    return m_tNextState;
}

_float CPlayer_LockonJustEvadeState::End()
{
	m_pGameInstance->SetGameSpeed(1.f);
	m_Desc->isInvincible = false;

    return m_fNextBlendRatio;
}

CPlayer_LockonJustEvadeState* CPlayer_LockonJustEvadeState::Create(void* pArg)
{
    return new CPlayer_LockonJustEvadeState();
}

void CPlayer_LockonJustEvadeState::Free()
{
	__super::Free();
}
