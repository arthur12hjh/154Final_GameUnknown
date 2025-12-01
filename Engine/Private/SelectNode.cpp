#include "SelectNode.h"

CSelectNode::CSelectNode() : CBehaviorNode()
{
}

HRESULT CSelectNode::Initialize_Prototype(CBehaviorTree* pOwnerTree)
{
    m_eNodeType = BEHAVIOR_NODE_TYPE::SELECTOR;
    if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
        return E_FAIL;

    return S_OK;
}


CBehaviorNode::NODE_STATE CSelectNode::Update(_float fTimeDelta)
{
    _bool bIsDecorator = true;
    for (auto& iter : m_Decorators)
    {
        if (NODE_STATE::FAIL == iter->Update(fTimeDelta))
        {
            bIsDecorator = false;
            m_iIndex = 0;
            return NODE_STATE::FAIL;
        }
    }

    while (bIsDecorator)
    {
        NODE_STATE State = m_Actions[m_iIndex]->Update(fTimeDelta);
        switch (State)
        {
        case NODE_STATE::COMPLETE:
            m_iIndex = 0;
            return NODE_STATE::COMPLETE;
        case NODE_STATE::FAIL:
            m_iIndex++;
            break;
        default:
            return NODE_STATE::RUNNING;
        }

        if (m_Actions.size() <= m_iIndex)
        {
            m_iIndex = 0;
            return NODE_STATE::FAIL;
        }
    }
    
    return NODE_STATE::RUNNING;
}

void CSelectNode::Bind_BehaviorNode(CBehaviorNode* pNode)
{
    if (nullptr == pNode)
        return;

    switch (pNode->GetNodeType())
    {
    case BEHAVIOR_NODE_TYPE::SELECTOR:
    case BEHAVIOR_NODE_TYPE::SQUENCE:
    case BEHAVIOR_NODE_TYPE::TASK:
        m_Actions.push_back(pNode);
        break;

    case BEHAVIOR_NODE_TYPE::SERVICE:
        m_Services.push_back(pNode);
        break;

    case BEHAVIOR_NODE_TYPE::DECORATOR:
        m_Decorators.push_back(pNode);
        break;
    }
}

CSelectNode* CSelectNode::Create(CBehaviorTree* pOwnerTree)
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

    for (auto& iter : m_Actions)
        Safe_Release(iter);
}
