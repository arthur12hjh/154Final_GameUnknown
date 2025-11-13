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

_bool CBehaviorNode::Update(_float fTimeDelta)
{
	return false;
}

CBehaviorNode* CBehaviorNode::Clone(void* pArg)
{
	return nullptr;
}

void CBehaviorNode::Free()
{
	__super::Free();
}