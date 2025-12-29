#include "pch.h"
#include "BeatSaberFsm.h"

#include "State.h"


CBeatSaberFsm::CBeatSaberFsm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CStateMachine(pDevice, pContext)
{
}

HRESULT CBeatSaberFsm::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBeatSaberFsm::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pCurrentState->Is_FinishedState())
	{
		Change_State(TEXT("Idle"));
	}
}

HRESULT CBeatSaberFsm::Change_State(const WCHAR* LayerTag, void* pArg, _bool bIsForce)
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
	m_eMonsterState = BEATSABER_STATE(m_pCurrentState->GetStateID());

	return S_OK;
}

HRESULT CBeatSaberFsm::Add_State(const WCHAR* StateTag, CState* pNewState)
{
	auto iter = m_pStates.find(StateTag);
	if (iter == m_pStates.end())
	{
		m_pStates.emplace(StateTag, pNewState);
	}

	return S_OK;
}

CBeatSaberFsm* CBeatSaberFsm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBeatSaberFsm* pBeatSaberFSM = new CBeatSaberFsm(pDevice, pContext);
	if (FAILED(pBeatSaberFSM->Initialize_Prototype()))
	{
		Safe_Release(pBeatSaberFSM);
		MSG_BOX("Create Fail : Beat Saber Fsm");
	}
	return pBeatSaberFSM;
}

CComponent* CBeatSaberFsm::Clone(void* pArg)
{
	CBeatSaberFsm* pBeatSaberFSM = new CBeatSaberFsm(*this);
	if (FAILED(pBeatSaberFSM->Initialize(pArg)))
	{
		Safe_Release(pBeatSaberFSM);
		MSG_BOX("Clone Fail : Beat Saber Fsm");
	}
	return pBeatSaberFSM;
}

void CBeatSaberFsm::Free()
{
	__super::Free();
}
