#include "pch.h"
#include "Task_Dead.h"

#include "BehaviorTree.h"
#include "ContainerObject.h"

CTask_Dead::CTask_Dead()
{
}

HRESULT CTask_Dead::Initialize_Prototype(const CBehaviorTree* pOwnerTree)
{
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

    return S_OK;
}

CBehaviorNode::NODE_STATE CTask_Dead::Update(_float fTimeDelta)
{
    // 애니메이션을 받아서 재생한다.





    return NODE_STATE::RUNNING;
}

CTask_Dead* CTask_Dead::Create(const CBehaviorTree* pOwnerTree)
{
    CTask_Dead* pTask_Dead = new CTask_Dead();
    if (FAILED(pTask_Dead->Initialize_Prototype(pOwnerTree)))
    {
        Safe_Release(pTask_Dead);
        MSG_BOX("Create Fail : Task Dead");
    }
    return pTask_Dead;
}

void CTask_Dead::Free()
{
    __super::Free();
}
