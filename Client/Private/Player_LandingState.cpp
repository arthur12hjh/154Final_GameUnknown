#include "pch.h"
#include "Player_LandingState.h"

#include "Player.h"
#include "GameInstance.h"

#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_WalkState.h"
#pragma endregion

CPlayer_LandingState::CPlayer_LandingState()
{
}

void CPlayer_LandingState::Start(void* pArg)
{
	m_pPlayer->Set_Animation("Proto_Jump_End", false);
	m_eType = JUMP_TYPE::IDLE;
}

CPlayerState* CPlayer_LandingState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

	if (true == isAnimFinished)
	{
		switch (m_eType)
		{
		case JUMP_TYPE::RUN:
			m_pNextState = CPlayer_WalkState::Create(nullptr);
			break;

		case JUMP_TYPE::IDLE:
			m_pNextState = CPlayer_IdleState::Create(nullptr);
			break;
		default:
			break;
		}
	}

	if (JUMP_TYPE::RUN == m_eType)
	{

	}

	return m_pNextState;
}

void CPlayer_LandingState::End()
{
}

CPlayer_LandingState* CPlayer_LandingState::Create(void* pArg)
{
	return new CPlayer_LandingState();
}

void CPlayer_LandingState::Free()
{
	__super::Free();
}
