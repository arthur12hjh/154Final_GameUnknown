#include "pch.h"
#include "Task_GorillaHit.h"

CTask_GorillaHit::CTask_GorillaHit() : CTask()
{
}

HRESULT CTask_GorillaHit::Initialize_Prototype(const CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_GorillaHit::Update(_float fTimeDelta)
{
	return NODE_STATE::RUNNING;
}

CTask_GorillaHit* CTask_GorillaHit::Create(const CBehaviorTree* pOwnerTree)
{
	CTask_GorillaHit* pTask_GorillaHit = new CTask_GorillaHit();
	if (FAILED(pTask_GorillaHit->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_GorillaHit);
		MSG_BOX("Create Fail : Task Gorilla Hit");
	}
	return pTask_GorillaHit;
}

void CTask_GorillaHit::Free()
{
	__super::Free();
}
