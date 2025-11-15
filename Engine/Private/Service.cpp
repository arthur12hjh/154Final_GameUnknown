#include "Service.h"

CService::CService() : CBehaviorNode()
{
}

HRESULT CService::Initialize_Prototype(const CBehaviorTree* pOwnerTree)
{
	m_eNodeType = BEHAVIOR_NODE_TYPE::SERVICE;
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	return S_OK;
}

CBehaviorNode::NODE_STATE CService::Update(_float fTimeDelta)
{
	return NODE_STATE::COMPLETE;
}

void CService::Free()
{
	__super::Free();
}
