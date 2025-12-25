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
	if (false == m_pBlackBoard->IsPhaseLastAttack())
	{
		if (false == m_pBlackBoard->IsAttackEnable() || m_pBlackBoard->bIsExcution())
		{
			if (CBossBlackBoard::BOSS_STATE::INTERACTION_ATTACK == m_pBlackBoard->GetCurState())
				int a = 10;

			return NODE_STATE::FAIL;
		}
	}

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
