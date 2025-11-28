#include "pch.h"
#include "Player_EvadeState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_EvadeState::CPlayer_EvadeState()
	: CPlayerState {}
{
}

void CPlayer_EvadeState::Start(void* pArg)
{
	m_eState = PLAYER_STATE::EVADE;

	/* 방향 */
	/* 1. 아무것도 안눌렀으면 백스텝 */
	/* 2. 방향키 하나라도 눌렀으면 그 방향으로 스텝. (이동 로직이랑 똑같이 카메라 look 기준으로)*/

	if (false == (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)))
	{
		m_pPlayer->Set_Animation("P_Eve_Peaceful_Evade_Backward", false);
		m_isBackStep = true;
	}
	else
	{
		_float fDegree = 0.f;

		if(m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
			fDegree = 0.f;
		else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
			fDegree = 90.f;
		else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
			fDegree = 180.f;
		else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
			fDegree = 270.f;

		_vector vCameraLook =  XMLoadFloat4(m_pGameInstance->Get_CamLook());
		//회전 벡터 하나 만들어.
		_matrix matRot = XMMatrixRotationY(XMConvertToRadians(fDegree));
		//카메라 룩 y 소거. 
		vCameraLook = XMVector3Normalize(XMVectorSetY(XMVector3TransformNormal(vCameraLook, matRot), 0.f));

		m_Desc->pPlayerTransform->Change_Look(vCameraLook);
		m_pPlayer->Set_Animation("P_Eve_Peaceful_Evade_Forward", false);
	}
}

PLAYER_TRANSITION_DESC CPlayer_EvadeState::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 1.f);
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	if(true == m_isBackStep && fAnimationRatio < 0.3f)
		m_Desc->pPlayerTransform->Go_Backward(fTimeDelta * 1.35f);
	else if(false == m_isBackStep && fAnimationRatio < 0.3f)
		m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 1.35f);

	if(fAnimationRatio >= 0.3f && 
		(m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
		 m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
		 m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
		 m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)))
		m_tNextState.eNextState = PLAYER_STATE::WALK;

	else if (true == isAnimFinished)
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	return m_tNextState;
}

void CPlayer_EvadeState::End()
{
}

CPlayer_EvadeState* CPlayer_EvadeState::Create(void* pArg)
{
	return new CPlayer_EvadeState();
}

void CPlayer_EvadeState::Free()
{
	__super::Free();
}
