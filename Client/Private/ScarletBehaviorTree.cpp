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
#include "Task_Idle.h"
#include "Task_ScarletDead.h"
#include "Task_ScarletMove.h"
#include "Task_ScarletAttack.h"

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

    auto pAttackSelect = CSelectNode::Create(this);
    if (nullptr == pAttackSelect)
        return E_FAIL;
    pAttackSelect->Bind_BehaviorNode(CDeco_AttackDelay::Create(this));
    pAttackSelect->Bind_BehaviorNode(CTask_ScarletAttack::Create(this));

    auto pBattleSelect = CSelectNode::Create(this);
    if (nullptr == pBattleSelect)
        return E_FAIL;

    pBattleSelect->Bind_BehaviorNode(CDeco_FindTarget::Create(this));

    pBattleSelect->Bind_BehaviorNode(pAttackSelect);
    pBattleSelect->Bind_BehaviorNode(CTask_ScarletMove::Create(this));

    auto pHitReactionSelector = CSelectNode::Create(this);
    if (nullptr == pHitReactionSelector)
        return E_FAIL;

    pHitReactionSelector->Bind_BehaviorNode(CTask_Theshold::Create(this));
    pHitReactionSelector->Bind_BehaviorNode(CTask_Hit::Create(this));
    pHitReactionSelector->Bind_BehaviorNode(CTask_Groggy::Create(this));

    auto ActionSelect = CSelectNode::Create(this);
    if (nullptr == pRootSelect)
        return E_FAIL;
    ActionSelect->Bind_BehaviorNode(CDeco_CheckAlive::Create(this, 10.f));
    ActionSelect->Bind_BehaviorNode(pHitReactionSelector);
    ActionSelect->Bind_BehaviorNode(pBattleSelect);
    ActionSelect->Bind_BehaviorNode(CTask_Idle::Create(this));

    pRootSelect->Bind_BehaviorNode(ActionSelect);
    pRootSelect->Bind_BehaviorNode(CTask_ScarletDead::Create(this));

    m_pRootNode = pRootSelect;
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
