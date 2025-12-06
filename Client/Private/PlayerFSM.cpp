#include "pch.h"

#include "PlayerFSM.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

#include "Player_IdleState.h"
#include "Player_IdleWalkEndState.h"

#include "Player_BattleIdleState.h"
#include "Player_BattleWalkEndState.h"

#include "Player_LockonIdleState.h"
#include "Player_IdleWalkState.h"
#include "Player_BattleWalkState.h"
#include "Player_LockonWalkState.h"
#include "Player_LightAttackState.h"
#include "Player_BetaChargingSlashState.h"
#include "Player_BetaTripletState.h"
#include "Player_JumpState.h"
#include "Player_BattleLandingState.h"
#include "Player_IdleLandingState.h"
#include "Player_LockonEvadeState.h"
#include "Player_BattleEvadeState.h"
#include "Player_HitState.h"
#include "Player_DrawHairpin.h"
#include "Player_SheatheHairpin.h"
//패링 관련 모션들은 상태 자세히 분리.
#include "Player_ParryEndState.h"
#include "Player_ParryState.h"
#include "Player_ParrySuccessState.h"
#include "Player_ParryGuardState.h"

CPlayerFSM::CPlayerFSM() 
	: m_pGameInstance { CGameInstance::GetInstance() }
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGameManager);

	//참조 용도
	m_pPlayer = CGameManager::GetInstance()->GetGameCharacter();
	Safe_Release(m_pPlayer);
	m_pPlayerDesc = m_pPlayer->Get_Desc();
}

PLAYER_STATE CPlayerFSM::Get_StateEnum()
{
	return m_pCurrentState->Get_State(); 
}

void CPlayerFSM::Clear_FSM()
{
	Safe_Release(m_pCurrentState);
	m_pCurrentState = nullptr;
}

void CPlayerFSM::Change_State(CPlayerState* pNext)
{
	if (!pNext) return;

	_float fBlendRatio =  m_pCurrentState->End();

	Safe_Release(m_pCurrentState);
	m_pCurrentState = pNext;

	m_pCurrentState->Start(nullptr, fBlendRatio);
}

HRESULT CPlayerFSM::Initialize(void* pArg)
{
	if (FAILED(Ready_State()))
		return E_FAIL;

	return S_OK;
}

void CPlayerFSM::Update(_float fTimeDelta)
{
	// 상태 업데이트
	PLAYER_TRANSITION_DESC Desc = m_pCurrentState->Update(fTimeDelta);
	// 상태 요청 기반 처리
	Handle_Transition(Desc);
	// 모드 전환 체크
	Evaluate_ModeTransitions(fTimeDelta, Desc);
}

CPlayerState* CPlayerFSM::Create_State(PLAYER_TRANSITION_DESC tDesc)
{
	if (false == Check_CanStateEnter(tDesc))
		return nullptr;

	switch (tDesc.eNextState)
	{
	case PLAYER_STATE::IDLE:
		switch (m_pPlayerDesc->ePlayerMode)
		{
		case PLAYER_MODE::IDLE:   return CPlayer_IdleState::Create(tDesc.pArg);
		case PLAYER_MODE::BATTLE: return CPlayer_BattleIdleState::Create(tDesc.pArg);
		case PLAYER_MODE::LOCKON: return CPlayer_LockonIdleState::Create(tDesc.pArg);
		}
		break;

	case PLAYER_STATE::WALK:
		switch (m_pPlayerDesc->ePlayerMode)
		{
		case PLAYER_MODE::IDLE:   return CPlayer_IdleWalkState::Create(tDesc.pArg);
		case PLAYER_MODE::BATTLE: return CPlayer_BattleWalkState::Create(tDesc.pArg);
		case PLAYER_MODE::LOCKON: 
			if (m_pCurrentState->Get_State() == PLAYER_STATE::WALK ||
				m_pCurrentState->Get_State() == PLAYER_STATE::WALK_END)
			{
				_bool isRunning = { true };
				tDesc.pArg = &isRunning;
				return CPlayer_LockonWalkState::Create(tDesc.pArg);
			}
			else
				return CPlayer_LockonWalkState::Create(tDesc.pArg);
		}
		break;

	case PLAYER_STATE::WALK_END:
		switch (m_pPlayerDesc->ePlayerMode)
		{
		case PLAYER_MODE::IDLE:   return CPlayer_IdleWalkEndState::Create(tDesc.pArg);
		case PLAYER_MODE::BATTLE: return CPlayer_BattleWalkEndState::Create(tDesc.pArg);
		//락온은 idle로 전환시킨다.
		case PLAYER_MODE::LOCKON: return CPlayer_LockonIdleState::Create(tDesc.pArg);
		}
		break;

	case PLAYER_STATE::LANDING:
		switch (m_pPlayerDesc->ePlayerMode)
		{
		case PLAYER_MODE::IDLE:   return CPlayer_IdleLandingState::Create(tDesc.pArg);
		case PLAYER_MODE::BATTLE: return CPlayer_BattleLandingState::Create(tDesc.pArg);
		case PLAYER_MODE::LOCKON: return CPlayer_BattleLandingState::Create(tDesc.pArg);
		}
		break;
	case PLAYER_STATE::EVADE:
		switch (m_pPlayerDesc->ePlayerMode)
		{
		case PLAYER_MODE::IDLE:   
			if (m_pCurrentState->Get_State() == PLAYER_STATE::EVADE)
				return CPlayer_BattleEvadeState::Create(tDesc.pArg);
			else
				return CPlayer_IdleWalkState::Create(tDesc.pArg);

		case PLAYER_MODE::BATTLE: 
			if (m_pCurrentState->Get_State() == PLAYER_STATE::EVADE)
				return CPlayer_BattleWalkState::Create(tDesc.pArg);
			else
				return CPlayer_BattleEvadeState::Create(tDesc.pArg);

		case PLAYER_MODE::LOCKON: 
			if (m_pCurrentState->Get_State() == PLAYER_STATE::EVADE)
				return CPlayer_LockonWalkState::Create(tDesc.pArg);
			else
				return CPlayer_LockonEvadeState::Create(tDesc.pArg);

		}
		break;

	case PLAYER_STATE::JUMP:
		switch (m_pPlayerDesc->ePlayerMode)
		{
		case PLAYER_MODE::IDLE:   return CPlayer_JumpState::Create(tDesc.pArg);
		case PLAYER_MODE::BATTLE: return CPlayer_JumpState::Create(tDesc.pArg);
		case PLAYER_MODE::LOCKON: return CPlayer_JumpState::Create(tDesc.pArg);
		}
		break;

	case PLAYER_STATE::DRAW_HAIRPIN:
		return CPlayer_DrawHairpin::Create(tDesc.pArg);
		break;

	case PLAYER_STATE::SHEATHE_HAIRPIN:
		return CPlayer_SheatheHairpin::Create(tDesc.pArg);
		break;

	case PLAYER_STATE::LIGHT_ATTACK:
		if (m_pCurrentState->Get_State() == PLAYER_STATE::LIGHT_ATTACK)
			return nullptr;
		return CPlayer_LightAttackState::Create(tDesc.pArg);
		break;

	case PLAYER_STATE::HIT:
		return CPlayer_HitState::Create(tDesc.pArg);
		break;

	case PLAYER_STATE::BETA_CHARGINGSLASH:
		if (m_pPlayerDesc->ePlayerMode == PLAYER_MODE::IDLE ||
			m_pCurrentState->Get_State() == PLAYER_STATE::BETA_CHARGINGSLASH || 
			m_pCurrentState->Get_State() == PLAYER_STATE::BETA_TRIPLET)
			return nullptr;

		return CPlayer_BetaChargingSlashState::Create(tDesc.pArg);
		break;

	case PLAYER_STATE::BETA_TRIPLET:
		if (m_pPlayerDesc->ePlayerMode == PLAYER_MODE::IDLE ||
			m_pCurrentState->Get_State() == PLAYER_STATE::BETA_CHARGINGSLASH ||
			m_pCurrentState->Get_State() == PLAYER_STATE::BETA_TRIPLET)
			return nullptr;
		return CPlayer_BetaTripletState::Create(tDesc.pArg);
		break;
	// 패리 자세
	case PLAYER_STATE::PARRY:
		switch (m_pPlayerDesc->ePlayerMode)
		{
		case PLAYER_MODE::IDLE: return nullptr;
		case PLAYER_MODE::BATTLE: return CPlayer_ParryState::Create(tDesc.pArg);
		case PLAYER_MODE::LOCKON: return CPlayer_ParryState::Create(tDesc.pArg);
		}
		break;
	// 패리 성공
	case PLAYER_STATE::PARRY_SUCCESS:
		switch (m_pPlayerDesc->ePlayerMode)
		{
		case PLAYER_MODE::IDLE: nullptr;
		case PLAYER_MODE::BATTLE: return CPlayer_ParrySuccessState::Create(tDesc.pArg);
		case PLAYER_MODE::LOCKON: return CPlayer_ParrySuccessState::Create(tDesc.pArg);
		}
		break;
	// 패리 끝
	case PLAYER_STATE::PARRY_END:
		switch (m_pPlayerDesc->ePlayerMode)
		{
		case PLAYER_MODE::IDLE: nullptr;
		case PLAYER_MODE::BATTLE: return CPlayer_ParryEndState::Create(tDesc.pArg);
		case PLAYER_MODE::LOCKON: return CPlayer_ParryEndState::Create(tDesc.pArg);
		}
		break;
	// 일반 패리
	case PLAYER_STATE::PARRY_GUARD:
		switch (m_pPlayerDesc->ePlayerMode)
		{
		case PLAYER_MODE::IDLE: nullptr;
		case PLAYER_MODE::BATTLE: return CPlayer_ParryGuardState::Create(tDesc.pArg);
		case PLAYER_MODE::LOCKON: return CPlayer_ParryGuardState::Create(tDesc.pArg);
		}
		break;

	default:
		break;
	}

	return nullptr;
}

void CPlayerFSM::Handle_Transition(PLAYER_TRANSITION_DESC& Desc)
{
	// 모드 전환 + 상태 전환
	if (Desc.isChangeMode)
	{
		if (false == m_pCurrentState->isTransferAble(Desc.eMode, Desc.eNextState))
			return;

		PLAYER_MODE ePrePlayerMode = m_pPlayerDesc->ePlayerMode;

		// 모드 변경
		m_pPlayerDesc->ePlayerMode = Desc.eMode;

		// 상태도 같이 바꾸고 싶으면
		if (Desc.eNextState != PLAYER_STATE::STATE_END)
		{
			CPlayerState* pNextState = Create_State(Desc);
			if (nullptr != pNextState)
			{
				if (ePrePlayerMode == PLAYER_MODE::IDLE && (m_pPlayerDesc->ePlayerMode == PLAYER_MODE::BATTLE || m_pPlayerDesc->ePlayerMode == PLAYER_MODE::LOCKON))
					m_pPlayerDesc->isWeaponVisible = true;
				if (m_pPlayerDesc->ePlayerMode == PLAYER_MODE::IDLE && (ePrePlayerMode == PLAYER_MODE::BATTLE || ePrePlayerMode == PLAYER_MODE::LOCKON))
					m_pPlayerDesc->isWeaponVisible = false;

				Change_State(pNextState);
			}
		}

		// 여기서 끝. 모드만 바꾸고 상태 유지했다면 그냥 return
		return;
	}

	// 모드는 그대로 상태만 전환하는 경우
	if (Desc.eNextState != PLAYER_STATE::STATE_END)
	{
		CPlayerState* pNextState = Create_State(Desc);
		if (nullptr != pNextState)
			Change_State(pNextState);
	}
}

void CPlayerFSM::Evaluate_ModeTransitions(_float fTimeDelta, PLAYER_TRANSITION_DESC& Desc)
{
	if (m_pPlayerDesc->ePlayerMode == PLAYER_MODE::IDLE)
	{
		if (true == m_pPlayerDesc->HasTarget &&
			m_pPlayerDesc->fCurrentMinDist < 40.0f)
		{
			m_pPlayerDesc->ePlayerMode = PLAYER_MODE::BATTLE;

			PLAYER_TRANSITION_DESC t{};
			t.eNextState = PLAYER_STATE::DRAW_HAIRPIN;

			CPlayerState* pNext = Create_State(t);
			if (nullptr != pNext)
				Change_State(pNext);
		}
	}

	// Battle to Idle 자동 전환 (적 없고 거리 멀어지면 5초 후 Idle)
	if (m_pPlayerDesc->ePlayerMode == PLAYER_MODE::BATTLE)
	{
		if ((false == m_pPlayerDesc->HasTarget || m_pPlayerDesc->fCurrentMinDist > 40.0f) && 
			 PLAYER_STATE::IDLE == m_pCurrentState->Get_State())
		{
			m_pPlayerDesc->fModeTimer += fTimeDelta;

			if (m_pPlayerDesc->fModeTimer >= 2.5f)
			{
				m_pPlayerDesc->fModeTimer = 0.f;
				m_pPlayerDesc->ePlayerMode = PLAYER_MODE::IDLE;

				PLAYER_TRANSITION_DESC t{};
				t.eNextState = PLAYER_STATE::SHEATHE_HAIRPIN; // 

				CPlayerState* pNext = Create_State(t);
				if (nullptr != pNext)
					Change_State(pNext);
			}
		}
		else
			m_pPlayerDesc->fModeTimer = 0.f;
	}

	// Lockon 모드에서 타겟이 사라진 경우, Battle로 되돌리기
	if (m_pPlayerDesc->ePlayerMode == PLAYER_MODE::LOCKON)
	{
		if (!m_pPlayerDesc->HasTarget ||
			m_pPlayerDesc->fCurrentMinDist > 30.0f)
		{
			m_pPlayerDesc->ePlayerMode = PLAYER_MODE::BATTLE;

			PLAYER_TRANSITION_DESC t{};
			t.eNextState = m_pCurrentState->Get_State();

			CPlayerState* pNext = Create_State(t);
			if (pNext)
				Change_State(pNext);
		}
	}

	// 락온 토글 입력 처리
	if (m_pPlayerDesc->isRequestLockonToggle)
	{
		if (m_pPlayerDesc->HasTarget && m_pPlayerDesc->ePlayerMode != PLAYER_MODE::LOCKON &&
			m_pPlayerDesc->fCurrentMinDist < 30.0f)
		{
			m_pPlayerDesc->ePlayerMode = PLAYER_MODE::LOCKON;

			PLAYER_TRANSITION_DESC t{};
			t.eNextState = m_pCurrentState->Get_State();
			//t.eNextState = PLAYER_STATE::IDLE;

			CPlayerState* pNext = Create_State(t);
			if (nullptr != pNext)
				Change_State(pNext);
		}

		m_pPlayerDesc->isRequestLockonToggle = false;
	}
}

_bool CPlayerFSM::Check_CanStateEnter(PLAYER_TRANSITION_DESC& Desc)
{
	switch (Desc.eNextState)
	{
	case PLAYER_STATE::BETA_CHARGINGSLASH:
		return CPlayer_BetaChargingSlashState::CanEnter(m_pPlayer, m_pPlayerDesc);
		break;

	case PLAYER_STATE::BETA_TRIPLET:
		return CPlayer_BetaTripletState::CanEnter(m_pPlayer, m_pPlayerDesc);
		break;

	default:
		return true;
	}

	return true;
}

HRESULT CPlayerFSM::Ready_State()
{
	PLAYER_TRANSITION_DESC Desc{};
	Desc.eMode = m_pPlayerDesc->ePlayerMode; //IDLE
	Desc.eNextState = PLAYER_STATE::IDLE;
	Desc.isChangeMode = false; // 초기화니 딱히 모드 변경 처리 필요 없음

	CPlayerState* pState = Create_State(Desc);
	if (!pState)
		return E_FAIL;

	pState->Start(nullptr, 1.2f);
	m_pCurrentState = pState;

	return S_OK;
}

CPlayerFSM* CPlayerFSM::Create()
{
	CPlayerFSM* pInstance = new CPlayerFSM();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CreatE Failed : PlayerFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerFSM::Free()
{
	__super::Free();
	
	Safe_Release(m_pCurrentState);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pGameManager);
}
