#include "Task.h"

CTask::CTask() : CBehaviorNode()
{
}

HRESULT CTask::Initialize_Prototype(const CBehaviorTree* pOwnerTree)
{
    m_eNodeType = BEHAVIOR_NODE_TYPE::TASK;
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

    return S_OK;
}

CBehaviorNode::NODE_STATE CTask::Update(_float fTimeDelta)
{
    return NODE_STATE::COMPLETE;
}

void CTask::Free()
{
    __super::Free();
}
