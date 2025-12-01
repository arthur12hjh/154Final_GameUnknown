#include "pch.h"
#include "PlayerBattleFSM.h"

#include "Player.h"
#include "Player_BattleIdleState.h"
#include "Player_BattleWalkState.h"
#include "Player_BattleWalkEndState.h"
#include "Player_JumpState.h"
#include "Player_LightAttackState.h"
#include "Player_BattleEvadeState.h"
#include "Player_HitState.h"
#include "Player_VendingInteractionState.h"
#include "Player_BetaChargingSlahsState.h"
#include "Player_BattleLandingState.h"

#include "GameManager.h"
#include "GameInstance.h"

CPlayerBattleFSM::CPlayerBattleFSM()
	: CPlayerFSM{}
{
}

HRESULT CPlayerBattleFSM::Initialize(void* pArg)
{
	if (FAILED(Ready_State()))
		return E_FAIL;

	return S_OK;
}

void CPlayerBattleFSM::Update(_float fTimeDelta)
{
	PLAYER_TRANSITION_DESC tState = m_pCurrentState->Update(fTimeDelta);
	
	//Battle to Idle 로직 체크 
	if (true == Check_BattleToIdle(fTimeDelta))
		return;

	// 아래는 상태에서 받은 다른 상태로의 전환 요청.
	if (true == tState.isChangeMode)
	{
		m_pPlayer->Change_PlayerMode(tState.eMode, tState.eNextState);
		m_fBattleToIdle = 0.f;

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

void CPlayerBattleFSM::Change_FSM(PLAYER_STATE eNextState)
{
	PLAYER_TRANSITION_DESC Desc;

	Desc.eNextState = eNextState;

	if (Desc.eNextState == PLAYER_STATE::JUMP ||
		Desc.eNextState == PLAYER_STATE::HIT || 
		Desc.eNextState == PLAYER_STATE::BETA_CHARGINGSLASH)
		return;

	Clear_FSM();

	if (Desc.eNextState == PLAYER_STATE::WALK_END ||
		Desc.eNextState == PLAYER_STATE::LANDING ||
		Desc.eNextState == PLAYER_STATE::EVADE)
		Desc.eNextState = PLAYER_STATE::IDLE;

	CPlayerState* pNextState = Create_State(Desc);
	pNextState->Start();
	m_pCurrentState = pNextState;
}

CPlayerState* CPlayerBattleFSM::Create_State(PLAYER_TRANSITION_DESC tTransitionDesc)
{ 
	switch (tTransitionDesc.eNextState)
	{
	case PLAYER_STATE::IDLE: return CPlayer_BattleIdleState::Create(tTransitionDesc.pArg);

	case PLAYER_STATE::WALK: return CPlayer_BattleWalkState::Create(tTransitionDesc.pArg);

	case PLAYER_STATE::WALK_END: return CPlayer_BattleWalkEndState::Create(tTransitionDesc.pArg);

	case PLAYER_STATE::JUMP: return CPlayer_JumpState::Create(tTransitionDesc.pArg);

	case PLAYER_STATE::LIGHT_ATTACK: return CPlayer_LightAttackState::Create(tTransitionDesc.pArg);

	case PLAYER_STATE::EVADE: return CPlayer_BattleEvadeState::Create(tTransitionDesc.pArg);

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

HRESULT CPlayerBattleFSM::Ready_State()
{
	CPlayer_BattleIdleState* pState = CPlayer_BattleIdleState::Create(nullptr);
	pState->Start();

	m_pCurrentState = pState;

	return S_OK;
}

_bool CPlayerBattleFSM::Check_BattleToIdle(_float fTimeDelta)
{
	_float fMinDist = m_pGameManager->Get_CurMinDist();
	_bool isLockon  = m_pGameManager->Get_Lockon();

	if (fMinDist > 30 && false == isLockon)
		m_fBattleToIdle += fTimeDelta;

	if (m_fBattleToIdle >= 5.f)
	{
		m_fBattleToIdle = 0.f;
		m_pPlayer->Change_PlayerMode(PLAYER_MODE::IDLE, m_pCurrentState->Get_State());
		return true;
	}

	return false;
}

CPlayerBattleFSM* CPlayerBattleFSM::Create()
{
	CPlayerBattleFSM* pInstance = new CPlayerBattleFSM();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed : CPlayerBattleFSM");
	}

	return pInstance;
}

void CPlayerBattleFSM::Free()
{
	__super::Free();
}
