#include "pch.h"
#include "Task_Wait.h"

CTask_Wait::CTask_Wait() : CTask()
{
}

HRESULT CTask_Wait::Initialize_Prototype(const CBehaviorTree* pOwnerTree, _float fWaitTime)
{
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

    m_vTime.x = 0.f;
    m_vTime.y = fWaitTime;
    return S_OK;
}

CBehaviorNode::NODE_STATE CTask_Wait::Update(_float fTimeDelta)
{
    m_vTime.x += fTimeDelta;

    if (m_vTime.y <= m_vTime.x)
    {
        m_vTime.x = 0.f;
        return NODE_STATE::COMPLETE;
    }

    return NODE_STATE::RUNNING;
}

CTask_Wait* CTask_Wait::Create(const CBehaviorTree* pOwnerTree, _float fWaitTime)
{
    CTask_Wait* pTask_Wait = new CTask_Wait();
    if (FAILED(pTask_Wait->Initialize_Prototype(pOwnerTree, fWaitTime)))
    {
        Safe_Release(pTask_Wait);
        MSG_BOX("Create Fail : Task Wait");
    }
    return pTask_Wait;
}

void CTask_Wait::Free()
{
    __super::Free();
}
