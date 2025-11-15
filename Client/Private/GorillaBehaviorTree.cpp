#include "pch.h"

#include "GorillaBehaviorTree.h"
#include "GorillaBlackBoard.h"

#include "SquenceNode.h"
#include "SelectNode.h"

#include "Task_Dead.h"
#include "Task_Idle.h"
#include "Task_GorillaAttack.h"

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
	m_pBlackBoard = CGorillaBlackBoard::Create();
	if (nullptr == m_pBlackBoard)
		return E_FAIL;

	return S_OK;
}

HRESULT CGorillaBehaviorTree::Ready_TreeNodes()
{
	// 여기서 생성해서 노드 구성
	auto pRootSquence = CSquenceNode::Create(this);
	if (nullptr == pRootSquence)
		return E_FAIL;

	pRootSquence->Bind_BehaviorNode(CTask_Idle::Create(this));
	pRootSquence->Bind_BehaviorNode(CTask_GorillaAttack::Create(this));

	m_pRootNode = pRootSquence;
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
