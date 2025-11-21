#include "pch.h"
#include "MonsterPatrolState.h"

CMonsterPatrolState::CMonsterPatrolState() :
    CState()
{
}

HRESULT CMonsterPatrolState::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CMonsterPatrolState::Start(void* pArg)
{
}

void CMonsterPatrolState::Update(_float fTimeDelta)
{
    //순찰같은거 있다면 이거 하게
}

void CMonsterPatrolState::End()
{
}

CMonsterPatrolState* CMonsterPatrolState::Create(void* pArg)
{
    CMonsterPatrolState* pMonsterPatrolState = new CMonsterPatrolState();
    if (FAILED(pMonsterPatrolState->Initialize(pArg)))
    {
        Safe_Release(pMonsterPatrolState);
        MSG_BOX("Create Fail : Monster Patrol State");
    }
    return pMonsterPatrolState;
}

void CMonsterPatrolState::Free()
{
    __super::Free();
}
