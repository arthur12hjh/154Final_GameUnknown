#include "pch.h"
#include "MonsterMimesisController.h"

#include "GameInstance.h"
#include "StateMachine.h"

#include "Nayitba.h"
#include "GameManager.h"
#include "TargetComponent.h"

#pragma region State
#include "MonsterIdleState.h"
#include "MonsterAttackState.h"
#include "MonsterMoveState.h"
#include "MonsterHitState.h"
#include "MonsterDeadState.h"
#include "MonsterStateMimesis.h"
#include "MonsterTranslationState.h"
#include "MonsterGroggyState.h"
#include "MonsterSuccessActionState.h"
#pragma endregion

#include "MonsterFSM.h"

CMonsterMimesisController::CMonsterMimesisController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CAIController(pDevice, pContext)
{
}

CMonsterMimesisController::CMonsterMimesisController(const CMonsterMimesisController& Prototype) :
	CAIController(Prototype)
{
}

HRESULT CMonsterMimesisController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterMimesisController::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	auto pNayitba = static_cast<CNayitba*>(m_pParent);
	auto pDefaultData = pNayitba->GetStaticMonsterData();
	m_pOwnerData = &pNayitba->GetMonsterData();

	m_fAttackDelay = pDefaultData->fAttackCoolTime;
	AttackCompleted(0.f);
	m_bIsMimesis = true;

	return S_OK;
}

void CMonsterMimesisController::Priority_Update(_float fTimeDelta)
{
}

void CMonsterMimesisController::Update(_float fTimeDelta)
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
				M_TranslationState.pTarget = m_pTargetCom->GetTarget();
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

void CMonsterMimesisController::Late_Update(_float fTimeDelta)
{
}

HRESULT CMonsterMimesisController::Render()
{
	return S_OK;
}

void CMonsterMimesisController::Damage(void* pArg)
{
	DEFAULT_DAMAGE_DESC* pDamageDesc = static_cast<DEFAULT_DAMAGE_DESC*>(pArg);

	auto pNayitba = static_cast<CNayitba*>(m_pParent);
	auto pAttackState = dynamic_cast<CMonsterAttackState*>(m_pFSM->GetCurrentState());
	if (0 >= m_pOwnerData->iCurrentHealth)
	{
		// 이거 죽는모션 나옴 죽으면 
		// 디졸브 이런 느낌의 이펙트 실행되고 삭제되게끔 제어할 예정
		m_pFSM->Change_State(TEXT("Dead"), pArg, true);
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
			if(m_bIsMimesis)
				m_bIsMimesis = false;

			m_pFSM->Change_State(TEXT("Hit"), pArg, true);
			m_pGameInstance->GamePauseDurationTime(0.3f, 0.8f, 5.f);
		}
			
	}
}

void CMonsterMimesisController::ActionSuccess(void* pArg)
{
	m_pFSM->Change_State(TEXT("ActionSuccess"), pArg, true);
}

HRESULT CMonsterMimesisController::Ready_Components()
{
	// 여기서 타겟 컴포넌트 만들어서 붙이자

	/* 시야 센서가 Controller에 달려있어야하나?*/
	CTargetComponent::TARGET_COMPONENT_DESC TargetComDesc = {};
	TargetComDesc.fRadius = 3.f;
	TargetComDesc.iNumPoints = 10.f;

	/* Prototype_Component_TargetComponent */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_TargetComponent"),
		TEXT("Com_TargetCom"), reinterpret_cast<CComponent**>(&m_pTargetCom), &TargetComDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterMimesisController::Ready_FSM()
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

	if (FAILED(m_pFSM->Add_State(TEXT("Mimesis"), CMonsterStateMimesis::Create(&Desc))))
		return E_FAIL;

	if (FAILED(m_pFSM->Add_State(TEXT("Translation"), CMonsterTranslationState::Create(&Desc))))
		return E_FAIL;

	if (FAILED(m_pFSM->Add_State(TEXT("ActionSuccess"), CMonsterSuccessActionState::Create(&Desc))))
		return E_FAIL;

	if (FAILED(m_pFSM->Add_State(TEXT("Groggy"), CMonsterGroggyState::Create(&Desc))))
		return E_FAIL;

	m_pFSM->Change_State(TEXT("Mimesis"));
	return S_OK;
}

void CMonsterMimesisController::Battle_Action(_float fTimeDelta)
{
	// 이거 배틀상태가 아니라면 안되게 하자
	auto pNayitba = static_cast<CNayitba*>(m_pParent);
	m_vAttackTime.x += fTimeDelta;

	//이거 너무 확확 바뀌니까 기가스도 인식하는거같음
	m_pTargetCom->Target_Search(pNayitba->GetTraceObejectList());
	auto pTarget = m_pTargetCom->GetTarget();

	_vector vOwnerPos = m_pParent->GetTransform()->Get_State(STATE::POSITION);
	_vector vTargetPos = pTarget->GetTransform()->Get_State(STATE::POSITION);
	_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vOwnerPos));

	// 이거 전부 배틀상태일때 입력이 되는거임
	// 공격을 하면 공격 입력
	// 공격이 가능해서 공격을 소비하면 이거 리셋하자
	// 상태가 바뀐다면 Bool Flag 리턴하자
	if (m_bIsMimesis)
	{
		CMonsterAttackState::MONSTER_ATTACK_DESC AttackStateDesc = {};
		AttackStateDesc.pTarget = pTarget;
		AttackStateDesc.AttackCompletedFunc = [&](_float fDelayTime) { this->AttackCompleted(fDelayTime); };
		m_pFSM->Change_State(TEXT("Attack"), &AttackStateDesc);
	}
	else
	{
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
					if (CMonsterFSM::MONSTER_STATE::HIT == eMonState)
						m_pFSM->Change_State(TEXT("Attack"), &AttackStateDesc, true);
					else
						m_pFSM->Change_State(TEXT("Attack"), &AttackStateDesc);
				}
				else
				{
					MoveAction(true);
				}
			}
			else
			{
				MoveAction(true);
			}
		}
	}
}

void CMonsterMimesisController::Default_Action(_float fTimeDelta)
{
	if (NAYTIBA_STATE::MIMESSIS == m_pOwnerData->eNaytibaState)
	{
		m_pFSM->Change_State(TEXT("Mimesis"));
	}
	else
	{
		MoveAction(false);
	}
}

void CMonsterMimesisController::AttackCompleted(_float fDelayTime)
{
	if(m_bIsMimesis)
		m_bIsMimesis = false;

	m_vAttackTime.x = 0.f;
	if (0.f == fDelayTime)
		m_vAttackTime.y = m_pGameInstance->Random(m_fAttackDelay - 3.f, m_fAttackDelay);
	else
		m_vAttackTime.y = m_pGameInstance->Random(fDelayTime - 3.f, m_fAttackDelay);
}

void CMonsterMimesisController::DelayAction(_float fDelayTime)
{
	m_vDelayTime = { 0.f, fDelayTime };
}

void CMonsterMimesisController::MoveAction(_bool bIsTarget)
{
	CMonsterMoveState::MOVE_STATE_DESC MoveStateDesc = {};
	//MoveStateDesc.PathFindingPoints = m_pTargetCom->GetPathFinding();
	MoveStateDesc.OnMoveCompleted = [&](_float fDelayTime) { this->DelayAction(fDelayTime); };

	if (bIsTarget)
		MoveStateDesc.pTarget = m_pTargetCom->GetTarget();
	else
		MoveStateDesc.pTarget = nullptr;

	m_pFSM->Change_State(TEXT("Move"), &MoveStateDesc);
}

CMonsterMimesisController* CMonsterMimesisController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterMimesisController* pMonsterMimessisController = new CMonsterMimesisController(pDevice, pContext);
	if (FAILED(pMonsterMimessisController->Initialize_Prototype()))
	{
		Safe_Release(pMonsterMimessisController);
		MSG_BOX("Create Fail : Monster Mimessis COntroller");
	}
	return pMonsterMimessisController;
}

CGameObject* CMonsterMimesisController::Clone(void* pArg)
{
	CMonsterMimesisController* pMonsterMimessisController = new CMonsterMimesisController(*this);
	if (FAILED(pMonsterMimessisController->Initialize(pArg)))
	{
		Safe_Release(pMonsterMimessisController);
		MSG_BOX("Clone Fail : Monster Mimessis COntroller");
	}
	return pMonsterMimessisController;
}

void CMonsterMimesisController::Free()
{
	__super::Free();

	Safe_Release(m_pFSM);
	Safe_Release(m_pTargetCom);
}
