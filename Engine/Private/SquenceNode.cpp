#include "SquenceNode.h"

CSquenceNode::CSquenceNode() : CBehaviorNode()
{
}

HRESULT CSquenceNode::Initialize_Prototype(const CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	return S_OK;
}

_bool CSquenceNode::Update(_float fTimeDelta)
{
	for (auto& iter : m_Decorators)
	{
		if (false == iter->Update(fTimeDelta))
			return false;
	}

	for (auto& iter : m_Services)
		iter->Update(fTimeDelta);

	for (auto& iter : m_Actions)
	{
		if (false == iter->Update(fTimeDelta))
			return false;
	}

	return true;
}

CSquenceNode* CSquenceNode::Create(const CBehaviorTree* pOwnerTree)
{
	CSquenceNode* pSquenceNode = new CSquenceNode();
	if (FAILED(pSquenceNode->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pSquenceNode);
		MSG_BOX("Create Fail : SquenceNode");
	}
	return pSquenceNode;
}

void CSquenceNode::Free()
{
	__super::Free();

	for (auto& iter : m_Decorators)
		Safe_Release(iter);

	for (auto& iter : m_Services)
		Safe_Release(iter);

	for (auto& iter : m_Actions)
		Safe_Release(iter);
}
