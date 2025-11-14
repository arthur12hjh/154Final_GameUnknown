#include "pch.h"
#include "Task_Move.h"

CTask_Move::CTask_Move() : CTask()
{
}

HRESULT CTask_Move::Initialize_Prototype(const CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	return S_OK;
}

CBehaviorNode::NODE_STATE CTask_Move::Update(_float fTimeDelta)
{
	// 블랙보드에 목표 지점 또는 타겟이 있을때 이동할 녀석

	return NODE_STATE::RUNNING;
}

CTask_Move* CTask_Move::Create(const CBehaviorTree* pOwnerTree)
{
	CTask_Move* pTask_Move = new CTask_Move();
	if (FAILED(pTask_Move->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pTask_Move);
		MSG_BOX("Create Fail : Task Move");
	}
	return pTask_Move;
}

void CTask_Move::Free()
{
	__super::Free();
}
