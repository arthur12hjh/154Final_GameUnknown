#include "pch.h"
#include "Deco_AttackRange.h"

CDeco_AttackRange::CDeco_AttackRange() : CDecorator()
{
}


HRESULT CDeco_AttackRange::Initialize_Prototype(const CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	return S_OK;
}

CBehaviorNode::NODE_STATE CDeco_AttackRange::Update(_float fTimeDelta)
{
    return NODE_STATE::RUNNING;
}

CDeco_AttackRange* CDeco_AttackRange::Create(const CBehaviorTree* pOwnerTree)
{
	CDeco_AttackRange* pTask_GorlliaAttackRange = new CDeco_AttackRange();
	if (FAILED(pTask_GorlliaAttackRange->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_GorlliaAttackRange);
		MSG_BOX("Create Fail : Task Gorilla Range");
	}
	return pTask_GorlliaAttackRange;
}

void CDeco_AttackRange::Free()
{
	__super::Free();
}
