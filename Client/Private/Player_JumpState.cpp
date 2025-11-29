#include "pch.h"
#include "Player_JumpState.h"

#include "Player.h"
#include "GameInstance.h"

#include "Player_BattleWalkState.h"

CPlayer_JumpState::CPlayer_JumpState()
	: CPlayerState {}
{
}

void CPlayer_JumpState::Start(void* pArg)
{
	m_eState = PLAYER_STATE::JUMP;

	m_pPlayer->Set_Animation("Proto_Jump_Start", false);
	m_Desc->pPlayerController->Set_Gravity(true, 17.f);
}

PLAYER_TRANSITION_DESC CPlayer_JumpState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
	{
		_vector vCameraLook = XMVector3Normalize(XMVectorSetY(XMLoadFloat4(m_pGameInstance->Get_CamLook()), 0.f));
		_bool isWalking = { false };

		m_fDegree = 0.f;

		if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
		{
			isWalking = true;

			if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
				m_fDegree -= 90.f;

			else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
				m_fDegree += 90.f;
		}

		else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
		{
			m_fDegree += 180.f;
			isWalking = true;

			if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
				m_fDegree += 90.f;

			else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
				m_fDegree -= 90.f;
		}

		else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
		{
			m_fDegree -= 90.f;

			isWalking = true;
		}

		else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
		{
			m_fDegree += 90.f;
			isWalking = true;
		}

		m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 0.6f);
	}

	if (false == m_Desc->pPlayerController->Get_Gravity())
	{
		if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_W) ||
			m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_S) ||
			m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_A) ||
			m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_D) ||
			m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
			m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
			m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
			m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
		{
			m_NextStateDesc.isLand = true;
			m_tNextState.pArg = &m_NextStateDesc;
			m_tNextState.eNextState = PLAYER_STATE::WALK;
		}
		else
		{
			m_tNextState.eNextState = PLAYER_STATE::LANDING;
		}

	}

	return m_tNextState;
}

void CPlayer_JumpState::End()
{
}

CPlayer_JumpState* CPlayer_JumpState::Create(void* pArg)
{
	return new CPlayer_JumpState();
}

void CPlayer_JumpState::Free()
{
	__super::Free();
}
