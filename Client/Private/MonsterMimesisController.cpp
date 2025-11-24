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

	auto pNayitba = static_cast<CNayitba*>(m_pOwner);
	auto pDefaultData = pNayitba->GetStaticMonsterData();
	m_pOwnerData = &pNayitba->GetMonsterData();

	m_fAttackDelay = pDefaultData->fAttackCoolTime;
	m_vAttackTime.y = m_pGameInstance->Random(2.f, m_fAttackDelay);

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
		if (m_vDelayTime.x >= m_vDelayTime.y)
		{
			if (NAYTIBA_STATE::BATTLE == m_pOwnerData->eNaytiba)
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
	if (0 >= m_pOwnerData->iCurrentHealth)
	{
		// 이거 죽는모션 나옴 죽으면 
		// 디졸브 이런 느낌의 이펙트 실행되고 삭제되게끔 제어할 예정
		m_bIsDead = true;
		m_pFSM->Change_State(TEXT("Dead"), pArg);
	}
	else
	{
		// 여기서 피격을 입력으로 피격 무조건 실행하게 하고 데미지도 들어가는데
		// 일단 입력을 넘기고 어떤 상태이냐에 대한 예외처리를 하자
		m_pFSM->Change_State(TEXT("Hit"), pArg);
		m_bIsMimesis = false;
	}
	
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
	Desc.pOwner = m_pOwner;

	auto pClone = m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Monster_FSM"), &Desc);
	if (nullptr == pClone)
		return E_FAIL;

	m_pFSM = static_cast<CStateMachine*>(pClone);

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

	m_pFSM->Change_State(TEXT("Mimesis"));
	return S_OK;
}

void CMonsterMimesisController::Battle_Action(_float fTimeDelta)
{
	// 이거 배틀상태가 아니라면 안되게 하자
	auto pNayitba = static_cast<CNayitba*>(m_pOwner);
	m_vAttackTime.x += fTimeDelta;

	//이거 너무 확확 바뀌니까 기가스도 인식하는거같음
	m_pTargetCom->Target_Search(pNayitba->GetTraceObejectList());
	auto pTarget = m_pTargetCom->GetTarget();

	_vector vOwnerPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector vTargetPos = pTarget->GetTransform()->Get_State(STATE::POSITION);
	_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vOwnerPos));

	// 이거 전부 배틀상태일때 입력이 되는거임
	// 공격을 하면 공격 입력
	// 공격이 가능해서 공격을 소비하면 이거 리셋하자
	// 상태가 바뀐다면 Bool Flag 리턴하자
	if (m_bIsMimesis)
	{
		CMonsterAttackState::MONSTER_ATTACK_DESC AttackStateDesc = {};
		AttackStateDesc.AttackCompletedFunc = [&](_float fDelayTime) { this->AttackCompleted(fDelayTime); };
		m_pFSM->Change_State(TEXT("Attack"), &AttackStateDesc);
	}
	else
	{
		if (m_vAttackTime.y <= m_vAttackTime.x)
		{
			if (fDistance <= m_pOwnerData->fAttackRange)
			{
				CMonsterAttackState::MONSTER_ATTACK_DESC AttackStateDesc = {};
				AttackStateDesc.AttackCompletedFunc = [&](_float fDelayTime) { this->AttackCompleted(fDelayTime); };
				m_pFSM->Change_State(TEXT("Attack"), &AttackStateDesc);
			}
		}
		else
		{
			MoveAction(true);
		}
	}
	
}

void CMonsterMimesisController::Default_Action(_float fTimeDelta)
{
	if (NAYTIBA_STATE::MIMESSIS == m_pOwnerData->eNaytiba)
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
	//m_vAttackTime.y = m_pGameInstance->Random(2.f, m_fAttackDelay);
	m_vAttackTime.y = m_pGameInstance->Random(15.f, 20.f);
}

void CMonsterMimesisController::DelayAction(_float fDelayTime)
{
	m_vDelayTime = { 0.f, fDelayTime };
}

void CMonsterMimesisController::MoveAction(_bool bIsTarget)
{
	CMonsterMoveState::MOVE_STATE_DESC MoveStateDesc = {};
	MoveStateDesc.PathFindingPoints = m_pTargetCom->GetPathFinding();
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
