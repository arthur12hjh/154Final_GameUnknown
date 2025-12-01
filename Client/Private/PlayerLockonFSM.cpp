#include "pch.h"
#include "PlayerLockonFSM.h"

#include "Player.h"
#include "Player_LockonIdleState.h"
#include "Player_LockonWalkState.h"
#include "Player_LockonEvadeState.h"
#include "Player_BattleWalkEndState.h"
#include "Player_JumpState.h"
#include "Player_LightAttackState.h"
#include "Player_HitState.h"
#include "Player_VendingInteractionState.h"
#include "Player_BetaChargingSlahsState.h"
#include "Player_BattleLandingState.h"

CPlayerLockonFSM::CPlayerLockonFSM()
	: CPlayerFSM {}
{
}

HRESULT CPlayerLockonFSM::Initialize(void* pArg)
{
	if (FAILED(Ready_State()))
		return E_FAIL;

	return S_OK;
}

void CPlayerLockonFSM::Update(_float fTimeDelta)
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

void CPlayerLockonFSM::Change_FSM(PLAYER_STATE eNextState)
{
	PLAYER_TRANSITION_DESC Desc;
	Desc.eNextState = eNextState;

	if (Desc.eNextState == PLAYER_STATE::JUMP ||
		Desc.eNextState == PLAYER_STATE::HIT ||
		Desc.eNextState == PLAYER_STATE::BETA_CHARGINGSLASH)
		return;
	
	Clear_FSM();

	//CHANGE 예외처리는 어쩔 수 없다.
	if (Desc.eNextState == PLAYER_STATE::WALK_END ||
		Desc.eNextState == PLAYER_STATE::LANDING ||
		Desc.eNextState == PLAYER_STATE::EVADE)
		Desc.eNextState = PLAYER_STATE::IDLE;

	CPlayerState* pNextState = Create_State(Desc);
	pNextState->Start();
	m_pCurrentState = pNextState;
}

CPlayerState* CPlayerLockonFSM::Create_State(PLAYER_TRANSITION_DESC tTransitionDesc)
{
	switch (tTransitionDesc.eNextState)
	{
	case PLAYER_STATE::IDLE: return CPlayer_LockonIdleState::Create(tTransitionDesc.pArg);
	//락온 walk도 수정
	case PLAYER_STATE::WALK: return CPlayer_LockonWalkState::Create(tTransitionDesc.pArg);

	//락온 walk end
	case PLAYER_STATE::WALK_END: return CPlayer_BattleWalkEndState::Create(tTransitionDesc.pArg);

	case PLAYER_STATE::JUMP: return CPlayer_JumpState::Create(tTransitionDesc.pArg);

	case PLAYER_STATE::LIGHT_ATTACK: return CPlayer_LightAttackState::Create(tTransitionDesc.pArg);

	//락온 evade
	case PLAYER_STATE::EVADE: return CPlayer_LockonEvadeState::Create(tTransitionDesc.pArg);

	case PLAYER_STATE::LANDING:  return CPlayer_BattleLandingState::Create(tTransitionDesc.pArg);

	case PLAYER_STATE::VENDING_INTERACTION: return CPlayer_VendingInteractionState::Create(tTransitionDesc.pArg);

	case PLAYER_STATE::HIT: break;

	case PLAYER_STATE::BETA_CHARGINGSLASH: return CPlayer_BetaChargingSlahsState::Create(tTransitionDesc.pArg);

		//미구현 상태
	case PLAYER_STATE::AERIAL_ATTACK: break;
	case PLAYER_STATE::STATE_END: break;
	default: break;
	}

	return nullptr;
}

HRESULT CPlayerLockonFSM::Ready_State()
{
	CPlayer_LockonIdleState* pState = CPlayer_LockonIdleState::Create(nullptr);
	pState->Start();

	m_pCurrentState = pState;

	return S_OK;
}

CPlayerLockonFSM* CPlayerLockonFSM::Create()
{
	CPlayerLockonFSM* pInstance = new CPlayerLockonFSM();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed : CPlayerLockOnFSM");
	}

	return pInstance;
}

void CPlayerLockonFSM::Free()
{
	__super::Free();
}
