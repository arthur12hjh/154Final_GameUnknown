#include "pch.h"
#include "NpcFSM.h"

#include "GameInstance.h"

#pragma region State
#include "NpcInteractionState.h"
#include "NpcPatrolState.h"
#pragma endregion

CNpcFSM::CNpcFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CStateMachine(pDevice, pContext)
{
}

HRESULT CNpcFSM::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CNpcFSM::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if (m_pCurrentState->Is_FinishedState())
    {
        Change_State(TEXT("Idle"));
    }
}

HRESULT CNpcFSM::Change_State(const WCHAR* LayerTag, void* pArg, _bool bIsForce)
{
	// 여기서 바로바꾸는게 아니라 현재 스테이트에서 가능한 동작을 받아다가
		// 변경한다.
	auto iter = m_pStates.find(LayerTag);
	if (iter == m_pStates.end())
		return E_FAIL;

	if (false == bIsForce)
	{
		if (iter->second == m_pCurrentState)
		{
			return S_OK;
		}
		else
		{
			if (m_pCurrentState)
			{
				if (false == m_pCurrentState->Is_EnableChange())
					return E_FAIL;
			}
		}
	}

	if (m_pCurrentState)
		m_pCurrentState->End();
	iter->second->Start(pArg, m_pCurrentState);

	m_pCurrentState = iter->second;
	m_eMonsterState = NPC_STATE(m_pCurrentState->GetStateID());

	return S_OK;
}

HRESULT CNpcFSM::Add_State(const WCHAR* StateTag, CState* pNewState)
{
	auto iter = m_pStates.find(StateTag);
	if (iter == m_pStates.end())
	{
		m_pStates.emplace(StateTag, pNewState);
	}

	return S_OK;
}

CNpcFSM* CNpcFSM::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNpcFSM* pNpcFSM = new CNpcFSM(pDevice, pContext);
	if (FAILED(pNpcFSM->Initialize_Prototype()))
	{
		Safe_Release(pNpcFSM);
		MSG_BOX("Create Fail : Npc Fsm");
	}
	return pNpcFSM;
}

CComponent* CNpcFSM::Clone(void* pArg)
{
	CNpcFSM* pNpcFSM = new CNpcFSM(*this);
	if (FAILED(pNpcFSM->Initialize(pArg)))
	{
		Safe_Release(pNpcFSM);
		MSG_BOX("Clone Fail : Npc Fsm");
	}
	return pNpcFSM;
}

void CNpcFSM::Free()
{
	__super::Free();
}
