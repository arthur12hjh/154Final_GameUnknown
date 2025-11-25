#include "pch.h"
#include "Player_JumpState.h"

#include "Player.h"
#include "GameInstance.h"

#pragma region TRANSFER_STATE

#include "Player_WalkState.h"
#include "Player_LandingState.h"
#include "Player_IdleState.h"

#pragma endregion

CPlayer_JumpState::CPlayer_JumpState()
	: CPlayerState {}
{
}

void CPlayer_JumpState::Start(void* pArg)
{
	m_pPlayer->Set_Animation("Proto_Jump_Start", false);
	m_Desc->pPlayerController->Set_Gravity(true, 17.f);

}

CPlayerState* CPlayer_JumpState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

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
			_bool isLand = true;
			m_pNextState = CPlayer_WalkState::Create(&isLand);
		}
		else
		{
			m_pNextState = CPlayer_LandingState::Create(nullptr);
		}

	}

	return m_pNextState;
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
