#include "BehaviorNode.h"
#include "GameInstance.h"

CBehaviorNode::CBehaviorNode() :
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CBehaviorNode::Initialize_Prototype(CBehaviorTree* pOwnerTree)
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

	Safe_Release(m_pGameInstance);
}