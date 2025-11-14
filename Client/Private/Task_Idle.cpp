#include "pch.h"
#include "Task_Idle.h"

#include "GameInstance.h"
#include "BehaviorTree.h"
#include "Entity.h"

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
	auto pOwner = static_cast<CEntity*>(m_pOwnerTree->GetOwner());
	pOwner->Set_AnimationIndex(1);
	pOwner->Play_Animation(fTimeDelta);

	_vector vPos = pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector vCamPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());

	_float fDistance = XMVectorGetX(XMVector3Length(vCamPos - vPos));
	if(fDistance <= 5.f)
		return NODE_STATE::COMPLETE;

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
