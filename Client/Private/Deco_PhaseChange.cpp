#include "pch.h"
#include "Deco_PhaseChange.h"

CPhaseChange::CPhaseChange() : CDecorator()
{
}

HRESULT CPhaseChange::Initialize_Prototype(const CBehaviorTree* pOwnerTree)
{
	if (FAILED(__super::Initialize_Prototype(pOwnerTree)))
		return E_FAIL;

	return S_OK;
}

CBehaviorNode::NODE_STATE CPhaseChange::Update(_float fTimeDelta)
{
	// 여기서 페이즈 체력 보고 다음 페이즈로


	return NODE_STATE::FAIL;
}

CPhaseChange* CPhaseChange::Create(const CBehaviorTree* pOwnerTree)
{
	CPhaseChange* pPhaseChange = new CPhaseChange();
	if (FAILED(pPhaseChange->Initialize_Prototype(pOwnerTree)))
	{
		Safe_Release(pPhaseChange);
		MSG_BOX("Create Fail : Phase Change");
	}
	return pPhaseChange;
}

void CPhaseChange::Free()
{
	__super::Free();
}
