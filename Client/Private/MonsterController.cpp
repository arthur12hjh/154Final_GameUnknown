#include "pch.h"
#include "MonsterController.h"

#include "GameInstance.h"
#include "StateMachine.h"

#include "Nayitba.h"
#include "GameManager.h"

#pragma region State
#include "MonsterIdleState.h"
#include "MonsterAttackState.h"
#include "MonsterMoveState.h"
#include "MonsterHitState.h"
#include "MonsterDeadState.h"
#include "MonsterGroggyState.h"
#include "MonsterTranslationState.h"
#pragma endregion

#include "MonsterFSM.h"
#include "Player.h"

CMonsterController::CMonsterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CAIController(pDevice, pContext)
{
}

CMonsterController::CMonsterController(const CMonsterController& Prototype) :
	CAIController(Prototype)
{
}

HRESULT CMonsterController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterController::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	auto pNayitba = static_cast<CNayitba*>(m_pParent);
	auto pDefaultData = pNayitba->GetStaticMonsterData();
	m_pOwnerData = &pNayitba->GetMonsterData();

	m_fAttackDelay = pDefaultData->fAttackCoolTime;
	AttackCompleted(0.f);

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	return S_OK;
}

void CMonsterController::Priority_Update(_float fTimeDelta)
{

}

void CMonsterController::Update(_float fTimeDelta)
{
	if (false == m_bIsDead)
	{
		m_vDelayTime.x += fTimeDelta;

		if (NAYTIBA_STATE::BATTLE == m_pOwnerData->eNaytibaState)
		{
			auto pNayitba = static_cast<CNayitba*>(m_pParent);
			if (NAYTIBA_STATE::DEFAULT == pNayitba->GetMonsterPreState())
			{
				CMonsterTranslationState::MONSTER_TRANSLATION_STATE M_TranslationState = {};
				M_TranslationState.szTranslationAnimName = "_BattleStart";
				M_TranslationState.szNextStateName = TEXT("Idle");
				M_TranslationState.pTarget = pNayitba->GetTarget();
				M_TranslationState.CompletedFunc = [&](const WCHAR* szNextStateName, void* pArg)
					{
						m_pFSM->Change_State(szNextStateName, pArg);
						m_vDelayTime = { 0.f, 0.5f };
					};
				M_TranslationState.pArg = nullptr;
				m_pFSM->Change_State(TEXT("Translation"), &M_TranslationState);
			}
		}

		if (m_vDelayTime.x >= m_vDelayTime.y)
		{
			if (NAYTIBA_STATE::BATTLE == m_pOwnerData->eNaytibaState)
				Battle_Action(fTimeDelta);
			else
				Default_Action(fTimeDelta);
		}
	}

	m_pFSM->Update(fTimeDelta);
}

void CMonsterController::Late_Update(_float fTimeDelta)
{
}

HRESULT CMonsterController::Render()
{
	return S_OK;
}

void CMonsterController::Damage(void* pDesc)
{
	auto pNayitba = static_cast<CNayitba*>(m_pParent);
	DEFAULT_DAMAGE_DESC* pDamageDesc = static_cast<DEFAULT_DAMAGE_DESC*>(pDesc);

	auto pAttackState = dynamic_cast<CMonsterAttackState *>(m_pFSM->GetCurrentState());
	if (0 >= m_pOwnerData->iCurrentHealth)
	{
		// 이거 죽는모션 나옴 죽으면 
		// 디졸브 이런 느낌의 이펙트 실행되고 삭제되게끔 제어할 예정
		m_pFSM->Change_State(TEXT("Dead"), pDesc, true);
		m_bIsDead = true;
	}
	else
	{
		// 여기서 피격을 입력으로 피격 무조건 실행하게 하고 데미지도 들어가는데
		// 일단 입력을 넘기고 어떤 상태이냐에 대한 예외처리를 하자
		_bool bIsHitAble = true;
		if (pAttackState)
		{
			// 나중에 여러 속성 추가할 예정
			const CHARACTER_SKILL_DESC* pDamageSKillDesc = static_cast<const CHARACTER_SKILL_DESC*>(pDamageDesc->pSkillData);

			if (SKILL_PROPERTY::PARRY & pDamageSKillDesc->eProPerty)
			{
				AttackCompleted(1.5f);
				if (0 >= m_pOwnerData->iCurrentStamina)
				{
					// 여기서 그로기 타임 주고 설정
					// 그로기 들어가기전에 패링 히트 애니메이션 재생후에 들어감
					// 원작은 뒤로 물러나면서 들어가는거 같음
					bIsHitAble = false;
					m_pFSM->Change_State(TEXT("Groggy"), nullptr, true);
				}
				else
				{
					if (false == pNayitba->bIsHitReaction())
						bIsHitAble = false;
				}
			}
			else
			{
				if (SKILL_PROPERTY::SUPERARMOR & pAttackState->GetSkillData()->eProPerty)
				{
					if (SKILL_TYPE::BETA_SKILL != pDamageSKillDesc->eSkillType)
					{
						bIsHitAble = false;
					}
				}
			}
		}
		else if (CMonsterFSM::MONSTER_STATE::GROGGY == m_pFSM->GetMonsterState())
			bIsHitAble = false;

		if (bIsHitAble)
		{
			m_pFSM->Change_State(TEXT("Hit"), pDesc, true);
		}
			
	}
}

void CMonsterController::ActionSuccess(void* pArg)
{
	// 이거 몬스터가 특정 공격일때 Success 함수 호출하면 불림
	// 근데 이거 이브 모션보고 이런형태일지 확인해야함
	// 은신형 몬스터는 잡기가 맞음
	m_pFSM->Change_State(TEXT("ActionSuccess"), pArg, true);
}

HRESULT CMonsterController::Ready_FSM()
{
	CStateMachine::STATEMACHINE_DESC Desc = {};
	Desc.pOwner = m_pParent;

	auto pClone = m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Monster_FSM"), &Desc);
	if (nullptr == pClone)
		return E_FAIL;

	m_pFSM = static_cast<CMonsterFSM*>(pClone);

	// 여기서 상태를 넣자
	// 특정몬스터가 상태를 가져야한다면 여기서 상태를 추가해줄수잇음
	if (FAILED(m_pFSM->Add_State(TEXT("Idle"), CMonsterIdleState::Create(&Desc))))
		return E_FAIL;

	if (FAILED(m_pFSM->Add_State(TEXT("Attack"), CMonsterAttackState::Create(&Desc))))
		return E_FAIL;

	if (FAILED(m_pFSM->Add_State(TEXT("Move"), CMonsterMoveState::Create(&Desc))))
		return E_FAIL;

	if (FAILED(m_pFSM->Add_State(TEXT("Dead"), CMonsterDeadState::Create(&Desc))))
		return E_FAIL;

	if (FAILED(m_pFSM->Add_State(TEXT("Hit"), CMonsterHitState::Create(&Desc))))
		return E_FAIL;

	if (FAILED(m_pFSM->Add_State(TEXT("Translation"), CMonsterTranslationState::Create(&Desc))))
		return E_FAIL;

	if (FAILED(m_pFSM->Add_State(TEXT("Groggy"), CMonsterGroggyState::Create(&Desc))))
		return E_FAIL;

	m_pFSM->Change_State(TEXT("Idle"));
	return S_OK;
}

void CMonsterController::Battle_Action(_float fTimeDelta)
{
	// 이거 배틀상태가 아니라면 안되게 하자
	auto pNayitba = static_cast<CNayitba*>(m_pParent);
	m_vAttackTime.x += fTimeDelta;

	//이거 너무 확확 바뀌니까 기가스도 인식하는거같음
	auto pTarget = pNayitba->GetTarget();
	if (nullptr == pTarget)
		return;

	_vector vOwnerPos = m_pParent->GetTransform()->Get_State(STATE::POSITION);
	_vector vTargetPos = pTarget->GetTransform()->Get_State(STATE::POSITION);
	_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vOwnerPos));

	// 이거 전부 배틀상태일때 입력이 되는거임
	// 공격을 하면 공격 입력
	// 공격이 가능해서 공격을 소비하면 이거 리셋하자
	// 상태가 바뀐다면 Bool Flag 리턴하자
	if (NAYTIBA_STATE::BATTLE == pNayitba->GetMonsterPreState())
	{
		if (m_vAttackTime.y <= m_vAttackTime.x)
		{
			if (fDistance <= m_pOwnerData->fAttackRange)
			{
				CMonsterAttackState::MONSTER_ATTACK_DESC AttackStateDesc = {};
				AttackStateDesc.pTarget = pTarget;
				AttackStateDesc.AttackCompletedFunc = [&](_float fDelayTime) { this->AttackCompleted(fDelayTime); };

				CMonsterFSM::MONSTER_STATE eMonState = m_pFSM->GetMonsterState();
				if(CMonsterFSM::MONSTER_STATE::HIT == eMonState)
					m_pFSM->Change_State(TEXT("Attack"), &AttackStateDesc, true);
				else
					m_pFSM->Change_State(TEXT("Attack"), &AttackStateDesc);
			}
			else
				MoveAction(true);
		}
		else
		{
			MoveAction(true);
		}
	}
}

void CMonsterController::Default_Action(_float fTimeDelta)
{
	// 여기서 대기 이후에 순찰하는걸로 하든 아무튼 대충 물어보고 
	// 로직을 결정하면 될거같음
	// 가만히 있는 녀석들도 있으니까
	// 플레그로 줘서 
	MoveAction(false);
}

void CMonsterController::AttackCompleted(_float fDelayTime)
{
	m_vAttackTime.x = 0.f;
	if (0.f == fDelayTime)
		m_vAttackTime.y = m_pGameInstance->Random(m_fAttackDelay - 3.f, m_fAttackDelay);
	else
		m_vAttackTime.y = m_pGameInstance->Random(fDelayTime - 3.f, m_fAttackDelay);
}

void CMonsterController::DelayAction(_float fDelayTime)
{
	m_vDelayTime = { 0.f, fDelayTime };
}

void CMonsterController::MoveAction(_bool bIsTarget)
{
	CMonsterMoveState::MOVE_STATE_DESC MoveStateDesc = {};
	//MoveStateDesc.PathFindingPoints = m_pTargetCom->GetPathFinding();
	MoveStateDesc.OnMoveCompleted = [&](_float fDelayTime) { this->DelayAction(fDelayTime); };

	if (bIsTarget)
	{
		auto pNayitba = static_cast<CNayitba*>(m_pParent);
		MoveStateDesc.pTarget = pNayitba->GetTarget();
	}
	else
		MoveStateDesc.pTarget = nullptr;

	m_pFSM->Change_State(TEXT("Move"), &MoveStateDesc);
}

CMonsterController* CMonsterController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterController* pMonsterController = new CMonsterController(pDevice, pContext);
	if (FAILED(pMonsterController->Initialize_Prototype()))
	{
		Safe_Release(pMonsterController);
		MSG_BOX("Create Fail : Monster Controller");
	}
	return pMonsterController;
}

CGameObject* CMonsterController::Clone(void* pArg)
{
	CMonsterController* pMonsterController = new CMonsterController(*this);
	if (FAILED(pMonsterController->Initialize(pArg)))
	{
		Safe_Release(pMonsterController);
		MSG_BOX("Clone Fail : Monster Controller");
	}
	return pMonsterController;
}

void CMonsterController::Free()
{
	__super::Free();

	Safe_Release(m_pFSM);
}
