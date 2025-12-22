#include "pch.h"
#include "NpcPatrolState.h"

CNpcPatrolState::CNpcPatrolState() : CState()
{
}

HRESULT CNpcPatrolState::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CNpcPatrolState::Start(void* pArg, CState* pPreState)
{
    // 돌아다니는 상태를 제어하기위한 스테이트
    // 돌아다니는 경로를 받아서 Lerp 하면서 움직이자.
}

void CNpcPatrolState::Update(_float fTimeDelta)
{

}

void CNpcPatrolState::End()
{

}

CNpcPatrolState* CNpcPatrolState::Create(void* pArg)
{
    CNpcPatrolState* pNpcPatrolState = new CNpcPatrolState();
    if (FAILED(pNpcPatrolState->Initialize(pArg)))
    {
        Safe_Release(pNpcPatrolState);
        MSG_BOX("Create Fail : Npc Patrol State");
    }
    return pNpcPatrolState;
}

void CNpcPatrolState::Free()
{
    __super::Free();
}
