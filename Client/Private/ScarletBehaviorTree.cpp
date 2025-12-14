#include "pch.h"
#include "ScarletBehaviorTree.h"
#include "ScarletBlackBoard.h"

#include "SquenceNode.h"
#include "SelectNode.h"

#pragma region Behavior Node

// Condition Node
#pragma region Decorator
#include "Deco_CheckAlive.h"
#include "Deco_FindTarget.h"
#include "Deco_AttackDelay.h"
#pragma endregion

// Action Node
#pragma region Task
#include "Task_Dead.h"
#include "Task_Idle.h"
#include "Task_Move.h"
#include "Task_Groggy.h"
#include "Task_Hit.h"
#include "Task_Theshold.h"
#pragma endregion

#pragma endregion

CScarletBehaviorTree::CScarletBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CBehaviorTree(pDevice, pContext)
{
}

CScarletBehaviorTree::CScarletBehaviorTree(const CScarletBehaviorTree& Prototype) :
    CBehaviorTree(Prototype)
{
}

HRESULT CScarletBehaviorTree::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CScarletBehaviorTree::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_BlackBoard()))
        return E_FAIL;

    if (FAILED(Ready_TreeNodes()))
        return E_FAIL;

    return S_OK;
}

void CScarletBehaviorTree::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

HRESULT CScarletBehaviorTree::Ready_BlackBoard()
{
    CScarletBlackBoard::BOSS_BLACKBOARD_DESC pDesc = {};
    pDesc.pOwner = m_pOwner;

    m_pBlackBoard = CScarletBlackBoard::Create(&pDesc);
    if (nullptr == m_pBlackBoard)
        return E_FAIL;

    return S_OK;
}

HRESULT CScarletBehaviorTree::Ready_TreeNodes()
{
    // 여기서 생성해서 노드 구성
    auto pRootSelect = CSelectNode::Create(this);
    if (nullptr == pRootSelect)
        return E_FAIL;

    pRootSelect->Bind_BehaviorNode(CTask_Idle::Create(this));
    return S_OK;
}

CScarletBehaviorTree* CScarletBehaviorTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CScarletBehaviorTree* pScarlet_BehaviorTree = new CScarletBehaviorTree(pDevice, pContext);
    if (FAILED(pScarlet_BehaviorTree->Initialize_Prototype()))
    {
        Safe_Release(pScarlet_BehaviorTree);
        MSG_BOX("Create Fail : Scarlet BehaviorTree");
    }
    return pScarlet_BehaviorTree;
}

CComponent* CScarletBehaviorTree::Clone(void* pArg)
{
    CScarletBehaviorTree* pScarlet_BehaviorTree = new CScarletBehaviorTree(*this);
    if (FAILED(pScarlet_BehaviorTree->Initialize(pArg)))
    {
        Safe_Release(pScarlet_BehaviorTree);
        MSG_BOX("Clone Fail : Scarlet BehaviorTree");
    }
    return pScarlet_BehaviorTree;
}

void CScarletBehaviorTree::Free()
{
    __super::Free();
}
