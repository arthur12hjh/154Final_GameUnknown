#include "SelectNode.h"

CSelectNode::CSelectNode() : CBehaviorNode()
{
}

HRESULT CSelectNode::Initialize_Prototype(const CBehaviorTree* pOwnerTree)
{
    return S_OK;
}


_bool CSelectNode::Update(_float fTimeDelta)
{
    _bool bIsDecorator = true;
    for (auto& iter : m_Decorators)
    {
        if (false == iter->Update(fTimeDelta))
            bIsDecorator = false;
    }

    if (bIsDecorator)
        return m_Action[0]->Update(fTimeDelta);

    return m_Action[1]->Update(fTimeDelta);
}

CSelectNode* CSelectNode::Create(const CBehaviorTree* pOwnerTree)
{
    CSelectNode* pSelectNode = new CSelectNode();
    if (FAILED(pSelectNode->Initialize_Prototype(pOwnerTree)))
    {
        Safe_Release(pSelectNode);
        MSG_BOX("Create Fail : Select Node");
    }
    return pSelectNode;
}

void CSelectNode::Free()
{
    __super::Free();

    for (auto& iter : m_Decorators)
        Safe_Release(iter);

    for (auto& iter : m_Services)
        Safe_Release(iter);

    for (auto& iter : m_Action)
        Safe_Release(iter);
}
