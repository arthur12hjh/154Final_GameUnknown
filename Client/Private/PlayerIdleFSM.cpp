#include "pch.h"
#include "PlayerIdleFSM.h"

#include "Player.h"
#include "Player_IdleState.h"
#include "Player_IdleWalkState.h"
#include "Player_IdleWalkEndState.h"
#include "Player_IdleLandingState.h"
#include "Player_BattleEvadeState.h"
#include "Player_JumpState.h"
#include "Player_DrawHairpin.h"
#include "Player_SheatheHairpin.h"

CPlayerIdleFSM::CPlayerIdleFSM()
    : CPlayerFSM {}
{
}

HRESULT CPlayerIdleFSM::Initialize(void* pArg)
{
    if (FAILED(Ready_State()))
        return E_FAIL;

    return S_OK;
}

void CPlayerIdleFSM::Update(_float fTimeDelta)
{
	PLAYER_TRANSITION_DESC tState = m_pCurrentState->Update(fTimeDelta);

	if (true == tState.isChangeMode)
	{
		m_pPlayer->Change_PlayerMode(tState.eMode, tState.eNextState);
		return;
	}

	if (PLAYER_STATE::STATE_END != tState.eNextState)
	{
		m_pCurrentState->End();
		CPlayerState* pNextState = Create_State(tState);
		pNextState->Start(); 

		//끝내고 기존 상태 없애야 desc 값 넘겨줄 수 있음.
		Safe_Release(m_pCurrentState);
		m_pCurrentState = pNextState;
	}
}

void CPlayerIdleFSM::Change_FSM(PLAYER_STATE eNextState)
{
	PLAYER_TRANSITION_DESC Desc;
	Desc.eNextState = eNextState;

	if (Desc.eNextState == PLAYER_STATE::JUMP ||
		Desc.eNextState == PLAYER_STATE::HIT ||
		Desc.eNextState == PLAYER_STATE::BETA_CHARGINGSLASH)
		return;

	Clear_FSM();

	//idle이나 walk가 아니라면, idle로 강제 보정.
	if (!(Desc.eNextState == PLAYER_STATE::IDLE ||
		Desc.eNextState == PLAYER_STATE::WALK))
		Desc.eNextState = PLAYER_STATE::IDLE;

	CPlayerState* pNextState = Create_State(Desc);
	pNextState->Start();
	m_pCurrentState = pNextState;
}

CPlayerState* CPlayerIdleFSM::Create_State(PLAYER_TRANSITION_DESC tTransitionDesc)
{
	switch (tTransitionDesc.eNextState)
	{
	case PLAYER_STATE::IDLE: return CPlayer_IdleState::Create(tTransitionDesc.pArg);
	case PLAYER_STATE::WALK: return CPlayer_IdleWalkState::Create(tTransitionDesc.pArg);
	case PLAYER_STATE::EVADE: return CPlayer_BattleEvadeState::Create(tTransitionDesc.pArg);
	case PLAYER_STATE::JUMP: return CPlayer_JumpState::Create(tTransitionDesc.pArg);
	case PLAYER_STATE::LANDING: return CPlayer_IdleLandingState::Create(tTransitionDesc.pArg);
	case PLAYER_STATE::WALK_END: return CPlayer_IdleWalkEndState::Create(tTransitionDesc.pArg);
	case PLAYER_STATE::LIGHT_ATTACK: break;
	case PLAYER_STATE::VENDING_INTERACTION: break;
	case PLAYER_STATE::HIT: break;
	case PLAYER_STATE::BETA_CHARGINGSLASH: break;
	case PLAYER_STATE::DRAW_HAIRPIN: return CPlayer_DrawHairpin::Create(tTransitionDesc.pArg);
	case PLAYER_STATE::SHEATHE_HAIRPIN: return CPlayer_SheatheHairpin::Create(tTransitionDesc.pArg);
	case PLAYER_STATE::AERIAL_ATTACK: break;
	case PLAYER_STATE::STATE_END: break;
	default: break;
	}

	return nullptr;
}

HRESULT CPlayerIdleFSM::Ready_State()
{
	CPlayer_IdleState* pState = CPlayer_IdleState::Create(nullptr);
	pState->Start();

	m_pCurrentState = pState;

	return S_OK;
}

CPlayerIdleFSM* CPlayerIdleFSM::Create()
{
	CPlayerIdleFSM* pInstance = new CPlayerIdleFSM();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed : CPlayerIdleFSM");
	}

	return pInstance;
}

void CPlayerIdleFSM::Free()
{
	__super::Free();
}
