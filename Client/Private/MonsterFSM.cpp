#include "pch.h"
#include "MonsterFSM.h"

#include "GameInstance.h"

#pragma region State
#include "MonsterIdleState.h"
#include "MonsterAttackState.h"
#include "MonsterMoveState.h"
#include "MonsterHitState.h"
#include "MonsterDeadState.h"
#include "PlayerFSM.h"
#pragma endregion

CMonsterFSM::CMonsterFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CStateMachine(pDevice, pContext)
{
}

HRESULT CMonsterFSM::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	return S_OK;
}

void CMonsterFSM::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pCurrentState->Is_FinishedState())
	{
		Change_State(TEXT("Idle"));
	}
}

HRESULT CMonsterFSM::Change_State(const WCHAR* LayerTag, void* pArg)
{
	// 여기서 바로바꾸는게 아니라 현재 스테이트에서 가능한 동작을 받아다가
	// 변경한다.
	if ( m_pCurrentState)
	{
		if (false == m_pCurrentState->Is_EnableChange())
			return E_FAIL;
	}

	auto iter = m_pStates.find(LayerTag);
	if (iter == m_pStates.end())
		return E_FAIL;

	if (iter->second == m_pCurrentState)
		return E_FAIL;

	iter->second->Start(pArg, m_pCurrentState);

	if (m_pCurrentState)
		m_pCurrentState->End();

	m_pCurrentState = iter->second;

	return S_OK;
}

HRESULT CMonsterFSM::Add_State(const WCHAR* StateTag, CState* pNewState)
{
	auto iter = m_pStates.find(StateTag);
	if (iter == m_pStates.end())
	{
		m_pStates.emplace(StateTag, pNewState);
	}

	return S_OK;
}

HRESULT CMonsterFSM::Ready_State()
{
	//CState::STATE_DESC Desc = {};
	//Desc.pOwner = m_pOwner;

	//if(FAILED(Add_State(TEXT("Idle"), CMonsterIdleState::Create(&Desc))))
	//	return E_FAIL;

	//if (FAILED(Add_State(TEXT("Attack"), CMonsterAttackState::Create(&Desc))))
	//	return E_FAIL;

	//if (FAILED(Add_State(TEXT("Move"), CMonsterMoveState::Create(&Desc))))
	//	return E_FAIL;

	//if (FAILED(Add_State(TEXT("Dead"), CMonsterDeadState::Create(&Desc))))
	//	return E_FAIL;

	//if (FAILED(Add_State(TEXT("Hit"), CMonsterHitState::Create(&Desc))))
	//	return E_FAIL;

	//Change_State(TEXT("Idle"));
	return S_OK;
}

CMonsterFSM* CMonsterFSM::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterFSM* pMonsterFsm = new CMonsterFSM(pDevice, pContext);
	if (FAILED(pMonsterFsm->Initialize_Prototype()))
	{
		Safe_Release(pMonsterFsm);
		MSG_BOX("Create Fail : Monster Fsm");
	}
	return pMonsterFsm;
}

CComponent* CMonsterFSM::Clone(void* pArg)
{
	CMonsterFSM* pMonsterFsm = new CMonsterFSM(*this);
	if (FAILED(pMonsterFsm->Initialize(pArg)))
	{
		Safe_Release(pMonsterFsm);
		MSG_BOX("Clone Fail : Monster Fsm");
	}
	return pMonsterFsm;
}

void CMonsterFSM::Free()
{
	__super::Free();
}
