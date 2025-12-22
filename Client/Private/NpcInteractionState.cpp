#include "pch.h"
#include "NpcInteractionState.h"

CNpcInteractionState::CNpcInteractionState()
{
}

HRESULT CNpcInteractionState::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CNpcInteractionState::Start(void* pArg, CState* pPreState)
{
}

void CNpcInteractionState::Update(_float fTimeDelta)
{
}

void CNpcInteractionState::End()
{
}

CNpcInteractionState* CNpcInteractionState::Create(void* pArg)
{
    CNpcInteractionState* pNpcInteractionState = new CNpcInteractionState();
    if (FAILED(pNpcInteractionState->Initialize(pArg)))
    {
        Safe_Release(pNpcInteractionState);
        MSG_BOX("Create Fail : Npc Interaction State");
    }
    return pNpcInteractionState;
}

void CNpcInteractionState::Free()
{
    __super::Free();
}
