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
		m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Sprint_Forward_Start"), false, 1.f, 0.15f);
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
		m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Sprint_Backward_Start"), false, 1.f, 0.15f);
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
		m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Sprint_Left_Start"), false, 1.f, 0.15f);
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
		m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Sprint_Right_Start"), false, 1.f, 0.15f);

    m_isSprintStart = true;
}

PLAYER_TRANSITION_DESC CPlayer_LockonSprintState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
	{
		if (false == m_isSprintStart)
			m_pPlayer->Set_Animation(TEXT("Proto_Battle_Sprint"), true, 1.f, 0.2f);
		else
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Sprint_Forward_Start"), false, 1.f, 0.3f);

		m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 1.6f);

	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
	{
		if (false == m_isSprintStart)
			m_pPlayer->Set_Animation(TEXT("Proto_Battle_Sprint_Backward"), true, 1.f, 0.2f);
		else
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Sprint_Backward_Start"), false, 1.f, 0.3f);

		m_Desc->pPlayerTransform->Go_Backward(fTimeDelta * 1.6f);

	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
	{
		if (false == m_isSprintStart)
			m_pPlayer->Set_Animation(TEXT("Proto_Battle_Sprint_Left"), true, 1.f, 0.2f);
		else
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Sprint_Left_Start"), false, 1.f, 0.3f);

		m_Desc->pPlayerTransform->Go_Left(fTimeDelta * 1.6f);
	}
	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
	{
		if (false == m_isSprintStart)
			m_pPlayer->Set_Animation(TEXT("Proto_Battle_Sprint_Right"), true, 1.f, 0.2f);
		else
			m_pPlayer->Set_Animation(TEXT("Proto_Lockon_Sprint_Right_Start"), false, 1.f, 0.3f);

		m_Desc->pPlayerTransform->Go_Right(fTimeDelta * 1.6f);
	}
	else
	{
		m_tNextState.eNextState = PLAYER_STATE::SPRINT_END;
		m_tNextState.pArg = &m_eDirection;
	}

	if(true == isAnimFinished && true == m_isSprintStart)
		m_isSprintStart = false;

	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_1))
		m_tNextState.eNextState = PLAYER_STATE::BETA_TRIPLET;
	else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
		m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;
	else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_E) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E))
		m_tNextState.eNextState = PLAYER_STATE::PARRY;

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


//_vector vCameraLook = XMVector3Normalize(XMVectorSetY(XMLoadFloat4(m_pGameInstance->Get_CamLook()), 0.f));
   //_bool isWalking = { false };

   //m_fDegree = 0.f;

   //if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
   //{
   //    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
   //    {
   //        if(false == m_isSprintStart)
   //            m_pPlayer->Set_Animation(TEXT("Proto_Battle_Sprint"), true, 1.f, 0.3f);
   //        m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 1.6f);
   //        m_eDirection = PLAYER_DIRECTION::STRAIGHT;
   //        isWalking = true;
   //    }

   //    else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
   //    {
   //        if (false == m_isSprintStart)
   //            m_pPlayer->Set_Animation(TEXT("Proto_Battle_Sprint_Backward"), true, 1.f, 0.3f);
   //        m_Desc->pPlayerTransform->Go_Backward(fTimeDelta * 1.6f);
   //        m_eDirection = PLAYER_DIRECTION::BACKWARD;
   //        isWalking = true;
   //    }

   //    else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
   //    {
   //        if (false == m_isSprintStart)
   //            m_pPlayer->Set_Animation(TEXT("Proto_Battle_Sprint_Left"), true, 1.f, 0.3f);
   //        m_Desc->pPlayerTransform->Go_Left(fTimeDelta * 1.6f);
   //        m_eDirection = PLAYER_DIRECTION::LEFT;
   //        isWalking = true;
   //    }
   //    else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
   //    {
   //        if (false == m_isSprintStart)
   //            m_pPlayer->Set_Animation(TEXT("Proto_Battle_Sprint_Right"), true, 1.f, 0.3f);
   //        m_Desc->pPlayerTransform->Go_Right(fTimeDelta * 1.6f);
   //        m_eDirection = PLAYER_DIRECTION::RIGHT;
   //        isWalking = true;
   //    }
   //}