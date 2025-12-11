#include "pch.h"
#include "Deco_AttackDelay.h"

#include "BehaviorTree.h"
#include "BossBlackBoard.h"

CDeco_AttackDelay::CDeco_AttackDelay() : CDecorator()
{
}


HRESULT CDeco_AttackDelay::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());

	return S_OK;
}

CBehaviorNode::NODE_STATE CDeco_AttackDelay::Update(_float fTimeDelta)
{
	m_pBlackBoard->SetTargetDistacne();
	_float fDistance = m_pBlackBoard->GetTargetDistance();
	_float fATKRange = m_pBlackBoard->GetBossInfo()->fAttackRange;

	auto pState = m_pBlackBoard->GetCurState();
	if (CBossBlackBoard::BOSS_STATE::ATTACK != pState)
	{
		if (false == m_pBlackBoard->IsAttackEnable())
		{
		
			return NODE_STATE::FAIL;
		}
		else if (fDistance > fATKRange * 4.f)
		{
			if (m_pBlackBoard->IsAttackEnable())
			{
				m_pBlackBoard->ClearAttackTimer();
				return NODE_STATE::COMPLETE;
			}
		}
	}
	
	if (m_pBlackBoard->IsAttackEnable())
		m_pBlackBoard->ClearAttackTimer();

	return NODE_STATE::COMPLETE;
}

CDeco_AttackDelay* CDeco_AttackDelay::Create(CBehaviorTree* pOwnerTree)
{
	CDeco_AttackDelay* pTask_GorlliaAttackRange = new CDeco_AttackDelay();
	if (FAILED(pTask_GorlliaAttackRange->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_GorlliaAttackRange);
		MSG_BOX("Create Fail : Task Gorilla Range");
	}
	return pTask_GorlliaAttackRange;
}

void CDeco_AttackDelay::Free()
{
	__super::Free();

	Safe_Release(m_pBlackBoard);
}
