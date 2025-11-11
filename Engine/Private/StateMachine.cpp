#include "StateMachine.h"

#include "State.h"

CStateMachine::CStateMachine()
{
}

HRESULT CStateMachine::Initialize(void* pArg)
{
	STATE_MACHINE_DESC* pDesc = static_cast<STATE_MACHINE_DESC*>(pArg);
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

void CStateMachine::Free()
{
	__super::Free();

	for (auto& iter : m_pStates)
		Safe_Release(iter.second);

	m_pStates.clear();
}
