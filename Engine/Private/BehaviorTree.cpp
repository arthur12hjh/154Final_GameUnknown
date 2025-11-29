#include "BehaviorTree.h"

#include "BlackBoard.h"
#include "BehaviorNode.h"

CBehaviorTree::CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CComponent(pDevice, pContext)
{
}

CBehaviorTree::CBehaviorTree(const CBehaviorTree& Prototype) :
    CComponent(Prototype)
{
}

HRESULT CBehaviorTree::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBehaviorTree::Initialize(void* pArg)
{
    BEHAVIORTREE_DESC* pDesc = static_cast<BEHAVIORTREE_DESC*>(pArg);
    m_pOwner = pDesc->pOwner;

    return S_OK;
}

void CBehaviorTree::Update(_float fTimeDelta)
{
    if(m_pRootNode)
        m_pRootNode->Update(fTimeDelta);
}

CBlackBoard* CBehaviorTree::GetBlackBoard() const
{
    if (nullptr == m_pBlackBoard)
        return nullptr;

    Safe_AddRef(m_pBlackBoard);
    return m_pBlackBoard;
}


CComponent* CBehaviorTree::Clone(void* pArg)
{
    
    return nullptr;
}

void CBehaviorTree::Free()
{
    __super::Free();

    Safe_Release(m_pRootNode);
    Safe_Release(m_pBlackBoard);
}
