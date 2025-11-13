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
    return S_OK;
}

void CBehaviorTree::Update(_float fTimeDelta)
{
}

CBlackBoard* CBehaviorTree::GetBlackBoard() const
{
    if (nullptr == m_pBlackBoard)
        return nullptr;

    Safe_AddRef(m_pBlackBoard);
    return m_pBlackBoard;
}

CBehaviorTree* CBehaviorTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBehaviorTree* pBehaviorTree = new CBehaviorTree(pDevice, pContext);
    if (FAILED(pBehaviorTree->Initialize_Prototype()))
    {
        Safe_Release(pBehaviorTree);
        MSG_BOX("Create Fail : Behavior Tree");
    }
    return pBehaviorTree;
}

CComponent* CBehaviorTree::Clone(void* pArg)
{
    CBehaviorTree* pBehaviorTree = new CBehaviorTree(*this);
    if (FAILED(pBehaviorTree->Initialize(pArg)))
    {
        Safe_Release(pBehaviorTree);
        MSG_BOX("Create Fail : Behavior Tree");
    }
    return pBehaviorTree;
}

void CBehaviorTree::Free()
{
    __super::Free();

    Safe_Release(m_pRootNode);
    Safe_Release(m_pBlackBoard);
}
