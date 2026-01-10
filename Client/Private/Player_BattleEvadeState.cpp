#include "pch.h"
#include "Player_BattleEvadeState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_BattleEvadeState::CPlayer_BattleEvadeState()
	: CPlayerState {}
{
}

void CPlayer_BattleEvadeState::Start(void* pArg, _float fBlendRatio)
{
	m_eState = PLAYER_STATE::EVADE;

	//if (PLAYER_MODE::BATTLE == m_Desc->ePlayerMode)
	//	m_fMaxRatio = 0.35f;
	//else
	m_fMaxRatio = 0.45f;

	/* 방향 */
	/* 1. 아무것도 안눌렀으면 백스텝 */
	/* 2. 방향키 하나라도 눌렀으면 그 방향으로 스텝. (이동 로직이랑 똑같이 카메라 look 기준으로)*/

	if (false == (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
		//m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)))
	{
		m_pPlayer->Set_Animation("Proto_Lockon_Evade_Backward", false, 1.7);
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
		m_pPlayer->Set_Animation("Proto_Lockon_Evade_Forward", false, 1.7f);
	}
}

PLAYER_TRANSITION_DESC CPlayer_BattleEvadeState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 1.f);
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	if(true == m_isBackStep && fAnimationRatio < 0.33f)
		m_Desc->pPlayerTransform->Go_Backward(fTimeDelta * 1.8f);
	else if(false == m_isBackStep && fAnimationRatio < 0.33f)
		m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 1.8f);

	if(fAnimationRatio > m_fMaxRatio - 0.15f)
		m_pPlayer->MotionTrailEnable(false);

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
	{
		if ((m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
			m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
			m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
			m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)) &&
			fAnimationRatio >= m_fMaxRatio - 0.08f)
			m_tNextState.eNextState = PLAYER_STATE::SPRINT;
		else if(fAnimationRatio >= m_fMaxRatio)
			m_tNextState.eNextState = PLAYER_STATE::EVADE;
	}

	if (fAnimationRatio >= m_fMaxRatio &&
		(m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
			m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
			m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
			m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)))
	{
		m_tNextState.eNextState = PLAYER_STATE::WALK;
		m_NextStateDesc.isEvade = true;
		m_tNextState.pArg = &m_NextStateDesc;
	}
	
	else if ((m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_E) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E)) && fAnimationRatio >= m_fMaxRatio)
		m_tNextState.eNextState = PLAYER_STATE::PARRY;

	else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_1)
		&& fAnimationRatio >= m_fMaxRatio)
	{
		m_tNextState.eNextState = PLAYER_STATE::BETA_TRIPLET;
		m_tNextState.isChangeMode = true;
		m_tNextState.eMode = PLAYER_MODE::BATTLE;
	}

	else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2)
		&& fAnimationRatio >= m_fMaxRatio)
	{
		m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;
		m_tNextState.isChangeMode = true;
		m_tNextState.eMode = PLAYER_MODE::BATTLE;
	}

	else if (true == isAnimFinished)
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	return m_tNextState;
}

_float CPlayer_BattleEvadeState::End()
{
	//m_pPlayer->SetVisibility(VISIBILITY::VISIBLE);

	return m_fNextBlendRatio;
}

CPlayer_BattleEvadeState* CPlayer_BattleEvadeState::Create(void* pArg)
{
	return new CPlayer_BattleEvadeState();
}

void CPlayer_BattleEvadeState::Free()
{
	__super::Free();
}
