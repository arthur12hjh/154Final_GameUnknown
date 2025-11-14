#include "pch.h"
#include "Task_GorillaAttack.h"

CTask_GorillaAttack::CTask_GorillaAttack() : CTask()
{
}

HRESULT CTask_GorillaAttack::Initialize_Prototype(const CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_GorillaAttack::Update(_float fTimeDelta)
{
	return NODE_STATE::RUNNING;
}

CTask_GorillaAttack* CTask_GorillaAttack::Create(const CBehaviorTree* pOwnerTree)
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
}
