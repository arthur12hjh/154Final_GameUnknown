#include "StateMachine.h"

#include "State.h"

CStateMachine::CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CComponent(pDevice, pContext)
{
}

HRESULT CStateMachine::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	STATEMACHINE_DESC* pDesc = static_cast<STATEMACHINE_DESC*>(pArg);
	m_pOwner = pDesc->pOwner;
	return S_OK;
}

void CStateMachine::Update(_float fTimeDelta)
{
	m_pCurrentState->Update(fTimeDelta);
}

CState* CStateMachine::Find_State(const WCHAR* StateTag)
{
	auto iter = m_pStates.find(StateTag);
	if (iter == m_pStates.end())
		return nullptr;

	return iter->second;
}

CComponent* CStateMachine::Clone(void* pArg)
{
	return nullptr;
}

void CStateMachine::Free()
{
	__super::Free();

	for (auto& iter : m_pStates)
		Safe_Release(iter.second);

	m_pStates.clear();
}
