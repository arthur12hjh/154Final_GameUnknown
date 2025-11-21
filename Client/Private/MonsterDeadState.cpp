#include "pch.h"
#include "MonsterDeadState.h"

CMonsterDeadState::CMonsterDeadState() :
	CState()
{
}

HRESULT CMonsterDeadState::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonsterDeadState::Start(void* pArg)
{
}

void CMonsterDeadState::Update(_float fTimeDelta)
{
}

void CMonsterDeadState::End()
{
}

CMonsterDeadState* CMonsterDeadState::Create(void* pArg)
{
	CMonsterDeadState* pMonsterDeadState = new CMonsterDeadState();
	if (FAILED(pMonsterDeadState->Initialize(pArg)))
	{
		Safe_Release(pMonsterDeadState);
		MSG_BOX("Create Fail : Monster Dead State");
	}

	return pMonsterDeadState;
}

void CMonsterDeadState::Free()
{
	__super::Free();
}
