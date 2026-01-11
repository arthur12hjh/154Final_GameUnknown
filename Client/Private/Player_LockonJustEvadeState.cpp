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
	m_pGameInstance->Active_RadialBlur(1.f, 16, 0.5f);

	m_eState = PLAYER_STATE::JUST_EVADE;
	m_Desc->isInvincible = true;
	
	m_pGameInstance->Manager_PlaySound(TEXT("EVE_BackStab1_Skill_start_01.wav"), CHANNELID::EFFECT, 3.f, 1.f);
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
	else
	{
		m_pPlayer->Set_Animation("Proto_Lockon_Sword_Just_Evade1_backward_3", false, 1.2f);
		m_eDirection = PLAYER_DIRECTION::BACKWARD;
	}

	//모트 켜
	m_pPlayer->MotionTrailEnable(true);
	m_pPlayer->SetVisibility(VISIBILITY::VISIBLE);
	m_pPlayer->MotionTrailRimLight(1.6f, 2.f, { 1.f, 0.5f, 0.5f, 1.f });
	m_pPlayer->MotionTrailCoolDown(0.1f);

	m_pGameInstance->SetGameSpeed(0.6f);
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

	if (PLAYER_DIRECTION::BACKWARD == m_eDirection && 0.4f <= fAnimationRatio)
	{
		m_fLerp += fTimeDelta * 10.f;

		if (m_fLerp >= 1.f)
			m_fLerp = 1.f;

		m_pGameInstance->SetGameSpeed(m_fLerp);
		if(0.45f <= fAnimationRatio)
			m_pPlayer->MotionTrailEnable(false);
	}
	else if (0.5f <= fAnimationRatio)
	{
		m_fLerp += fTimeDelta * 10.f;

		if (m_fLerp >= 1.f)
			m_fLerp = 1.f;
	
		m_pGameInstance->SetGameSpeed(m_fLerp);
		if (0.55f <= fAnimationRatio)
			m_pPlayer->MotionTrailEnable(false);
	}

	if (true == isAnimFinished)
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	//키입력을 이용한 캔슬도 필요함.

    return m_tNextState;
}

_float CPlayer_LockonJustEvadeState::End()
{
	m_pPlayer->MotionTrailEnable(false);
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
