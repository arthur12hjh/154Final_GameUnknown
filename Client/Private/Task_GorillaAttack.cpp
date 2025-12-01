#include "pch.h"
#include "Task_GorillaAttack.h"

#include "GameInstance.h"
#include "BossBlackBoard.h"
#include "GorillaBehaviorTree.h"
#include "Nayitba.h"

CTask_GorillaAttack::CTask_GorillaAttack() : CTask()
{
}

HRESULT CTask_GorillaAttack::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	if (nullptr == m_pOwner)
		m_pOwner = static_cast<CNayitba*>(m_pOwnerTree->GetOwner());

	if (nullptr == m_pBlackBoard)
		m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());

	m_iAttackCount.y = 4;
	m_fMaxDelayTime = 3.5f;
	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_GorillaAttack::Update(_float fTimeDelta)
{
	if (nullptr == m_pSkillData)
	{
		if (false == SelectRandomPattern())
			return NODE_STATE::FAIL;
	}

	CBossBlackBoard::BOSS_STATE eCurState = m_pBlackBoard->GetCurState();
	if (CBossBlackBoard::BOSS_STATE::HIT == eCurState || Compute_AttackCoolTime())
	{
		m_pSkillData = nullptr;
		return NODE_STATE::FAIL;
	}

	// 일단 여기서 고릴라 공격에대한 이동 처리
	AttackMoveAction();

	m_pBlackBoard->SetCurState(CBossBlackBoard::BOSS_STATE::ATTACK);
	_bool bIsFinished = m_pOwner->Play_Animation(fTimeDelta);
	if (bIsFinished)
	{
		m_pSkillData = nullptr;
		return NODE_STATE::COMPLETE;
	}
		
	return NODE_STATE::RUNNING;
}

_bool CTask_GorillaAttack::SelectRandomPattern()
{
	// 여기서 거리가 일단 멀어지면 날라오거나
	// 돌을 던지는 패턴을 하자

	m_pSkillData = m_pOwner->GetSkillData();

	if (nullptr == m_pSkillData)
		return false;

	m_iAttackCount.x++;
	m_szDebugAnimation = m_pSkillData->szAnimationName;
	m_pOwner->Set_Animation(m_pSkillData->szAnimationName, false, 1.0f, 1.2f, true);
	return true;
}

_bool CTask_GorillaAttack::AttackMoveAction()
{
	// 블랙보드에서 선택된 타겟을 가져오고 거리가 가까우면 이녀석은 거리를 좁히는 이동은 하지않는다.
	// 블랙 보드에서 거리를 받아와서 하자

	auto pNaytibaStaticData = m_pOwner->GetStaticMonsterData();
	if (nullptr == pNaytibaStaticData)
		return false;

	_float fDistance = m_pBlackBoard->GetTargetDistance();
	if (0 > fDistance)
		return false;

	if (pNaytibaStaticData->fAttackRange * 0.3f > fDistance)
		return false;
		
	// 여기서 선택된 스킬에 대한 정보를 처리한다.
	 



	return true;
}

_bool CTask_GorillaAttack::Compute_AttackCoolTime()
{
	if (m_iAttackCount.x >= m_iAttackCount.y)
	{
		m_iAttackCount.y = (_uint)m_pGameInstance->Random(2, m_iAttackCount.y);
		
		m_pBlackBoard->SetAttackDelay(m_pGameInstance->Random(2.f, m_fMaxDelayTime));
		m_iAttackCount.x = 0.f;
		return true;
	}
	return false;
}

CTask_GorillaAttack* CTask_GorillaAttack::Create(CBehaviorTree* pOwnerTree)
{
	CTask_GorillaAttack* pTask_GorillaAttack = new CTask_GorillaAttack();
	if (FAILED(pTask_GorillaAttack->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_GorillaAttack);
		MSG_BOX("Create Fail : Task Gorilla Attack");
	}
	return pTask_GorillaAttack;
}

void CTask_GorillaAttack::Free()
{
	__super::Free();

	Safe_Release(m_pBlackBoard);
}
