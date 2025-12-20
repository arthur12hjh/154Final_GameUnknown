#include "pch.h"
#include "Player_JumpState.h"

#include "Player.h"
#include "GameInstance.h"

#include "Player_BattleWalkState.h"

CPlayer_JumpState::CPlayer_JumpState(_bool isSprintJump)
	: CPlayerState {}
	, m_isSprintJump { isSprintJump }
{
}

void CPlayer_JumpState::Start(void* pArg, _float fBlendRatio)
{
	m_eState = PLAYER_STATE::JUMP;

	m_pPlayer->Set_Animation("Proto_Jump_Start", false);

	if (true == m_isSprintJump)
	{
		m_fScaleFactor = 1.7f;
		m_Desc->pPlayerController->Set_Gravity(true, 20.f);
	}

	else
	{
		m_fScaleFactor = 1.f;
		m_Desc->pPlayerController->Set_Gravity(true, 17.f);
	}
}

PLAYER_TRANSITION_DESC CPlayer_JumpState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
		m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 0.6f * m_fScaleFactor);

	else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
		m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 0.6f * m_fScaleFactor);


	if (false == m_Desc->pPlayerController->Get_Gravity())
	{
		if ((m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_W) ||
			m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_S) ||
			m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_A) ||
			m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_D) ||
			m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
			m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
			m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
			m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)) &&
			m_Desc->ePlayerMode != PLAYER_MODE::IDLE)
		{
			m_NextStateDesc.isLand = true;
			m_tNextState.pArg = &m_NextStateDesc;
			m_tNextState.eNextState = PLAYER_STATE::WALK;
			//쉬프트도 누르고 있었으면 뛰어
			if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
				m_tNextState.eNextState = PLAYER_STATE::SPRINT;
		}
		else
		{
			m_tNextState.eNextState = PLAYER_STATE::LANDING;
		}

	}

	return m_tNextState;
}

_float CPlayer_JumpState::End()
{
	return m_fNextBlendRatio;
}

CPlayer_JumpState* CPlayer_JumpState::Create(void* pArg)
{
	_bool isSprintJump = { false };

	if (nullptr != pArg)
	{
		PLAYER_JUMP_DESC* pDesc = static_cast<PLAYER_JUMP_DESC*>(pArg);
		isSprintJump = pDesc->isSprintJump;
	}


	return new CPlayer_JumpState(isSprintJump);
}

void CPlayer_JumpState::Free()
{
	__super::Free();
}
