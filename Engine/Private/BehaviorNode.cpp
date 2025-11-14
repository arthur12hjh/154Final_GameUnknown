#include "BehaviorNode.h"

CBehaviorNode::CBehaviorNode()
{
}

HRESULT CBehaviorNode::Initialize_Prototype(const CBehaviorTree* pOwnerTree)
{
	if (nullptr == pOwnerTree)
		return E_FAIL;

	m_pOwnerTree = pOwnerTree;
	return S_OK;
}

CBehaviorNode::NODE_STATE CBehaviorNode::Update(_float fTimeDelta)
{
	return NODE_STATE::COMPLETE;
}


void CBehaviorNode::Free()
{
	__super::Free();
}