#include "pch.h"

#include "GorillaBehaviorTree.h"
#include "GorillaBlackBoard.h"

#include "SquenceNode.h"
#include "SelectNode.h"

#pragma region Behavior Node

// Condition Node
#pragma region Decorator
#include "Deco_CheckAlive.h"
#pragma endregion

// Action Node
#pragma region Task
#include "Task_Dead.h"
#include "Task_Idle.h"
#pragma endregion


#include "Task_GorillaAttack.h"
#pragma endregion

CGorillaBehaviorTree::CGorillaBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CBehaviorTree(pDevice, pContext)
{
}

CGorillaBehaviorTree::CGorillaBehaviorTree(const CGorillaBehaviorTree& Prototype) :
	CBehaviorTree(Prototype)
{
}

HRESULT CGorillaBehaviorTree::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGorillaBehaviorTree::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_BlackBoard()))
		return E_FAIL;

	if (FAILED(Ready_TreeNodes()))
		return E_FAIL;

	return S_OK;
}

void CGorillaBehaviorTree::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

HRESULT CGorillaBehaviorTree::Ready_BlackBoard()
{
	CGorillaBlackBoard::BOSS_BLACKBOARD_DESC pDesc = {};
	pDesc.pOwner = m_pOwner;

	m_pBlackBoard = CGorillaBlackBoard::Create(&pDesc);
	if (nullptr == m_pBlackBoard)
		return E_FAIL;

	return S_OK;
}

HRESULT CGorillaBehaviorTree::Ready_TreeNodes()
{
	// 여기서 생성해서 노드 구성
	auto pRootSelect = CSelectNode::Create(this);
	if (nullptr == pRootSelect)
		return E_FAIL;

#pragma region Idle Node
	// Idle 노드 아래 구성할거임
	auto pAliveSquence = CSquenceNode::Create(this);
	if (nullptr == pAliveSquence)
		return E_FAIL;
	
	pAliveSquence->Bind_BehaviorNode(CDeco_CheckAlive::Create(this));
	pAliveSquence->Bind_BehaviorNode(CTask_Idle::Create(this));
#pragma endregion
	// 트리구성 1차 트리
	pRootSelect->Bind_BehaviorNode(pAliveSquence);
	pRootSelect->Bind_BehaviorNode(CTask_Dead::Create(this));
	
	
	
	// 
	//pRootSelect->Bind_BehaviorNode(CTask_GorillaAttack::Create(this));

	m_pRootNode = pRootSelect;
	return S_OK;
}

CGorillaBehaviorTree* CGorillaBehaviorTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGorillaBehaviorTree* pGorillaBehaviorTree = new CGorillaBehaviorTree(pDevice, pContext);
	if (FAILED(pGorillaBehaviorTree->Initialize_Prototype()))
	{
		Safe_Release(pGorillaBehaviorTree);
		MSG_BOX("Create Fail : Gorilla BehaviorTree");
	}
	return pGorillaBehaviorTree;
}

CComponent* CGorillaBehaviorTree::Clone(void* pArg)
{
	CGorillaBehaviorTree* pGorillaBehaviorTree = new CGorillaBehaviorTree(*this);
	if (FAILED(pGorillaBehaviorTree->Initialize(pArg)))
	{
		Safe_Release(pGorillaBehaviorTree);
		MSG_BOX("Clone Fail : Gorilla BehaviorTree");
	}
	return pGorillaBehaviorTree;
}

void CGorillaBehaviorTree::Free()
{
	__super::Free();
}
