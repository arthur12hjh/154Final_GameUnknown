#include "pch.h"
#include "Deco_FindTarget.h"

#include "BehaviorTree.h"
#include "BossBlackBoard.h"

CDeco_FindTarget::CDeco_FindTarget()
{
}

HRESULT CDeco_FindTarget::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

    m_pBlackBoard = static_cast<CBossBlackBoard*>(m_pOwnerTree->GetBlackBoard());

    return S_OK;
}

CBehaviorNode::NODE_STATE CDeco_FindTarget::Update(_float fTimeDelta)
{
    if (nullptr == m_pBlackBoard->GetTarget())
        return NODE_STATE::FAIL;

    return NODE_STATE::COMPLETE;
}

CDeco_FindTarget* CDeco_FindTarget::Create(CBehaviorTree* pOwnerTree)
{
    CDeco_FindTarget* pDeco_FindTarget = new CDeco_FindTarget();
    if (FAILED(pDeco_FindTarget->Initialize_Prototype(pOwnerTree)))
    {
        Safe_Release(pDeco_FindTarget);
        MSG_BOX("Create Fail : Deco Find Target");
    }
    return pDeco_FindTarget;
}

void CDeco_FindTarget::Free()
{
    __super::Free();

    Safe_Release(m_pBlackBoard);
}
