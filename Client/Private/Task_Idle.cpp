#include "pch.h"
#include "Task_Idle.h"

CTask_Idle::CTask_Idle() : CTask()
{
}

HRESULT CTask_Idle::Initialize_Prototype(const CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_Idle::Update(_float fTimeDelta)
{
	// 여기서 블렉보드 또는 다른곳의 상태가 바뀌면 Complete 호출해서 사용
	return NODE_STATE::RUNNING;
}

CTask_Idle* CTask_Idle::Create(const CBehaviorTree* pOwnerTree)
{
	CTask_Idle* pTask_Idle = new CTask_Idle();
	if (FAILED(pTask_Idle->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_Idle);
		MSG_BOX("Create Fail : Task Idle");
	}
	return pTask_Idle;
}

void CTask_Idle::Free()
{
	__super::Free();
}
