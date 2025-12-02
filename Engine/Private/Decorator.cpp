#include "Decorator.h"

CDecorator::CDecorator() : CBehaviorNode()
{
}


HRESULT CDecorator::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
    m_eNodeType = BEHAVIOR_NODE_TYPE::DECORATOR;
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

    return S_OK;
}

CBehaviorNode::NODE_STATE  CDecorator::Update(_float fTimeDelta)
{
    return NODE_STATE::COMPLETE;
}

void CDecorator::Free()
{
    __super::Free();
}
