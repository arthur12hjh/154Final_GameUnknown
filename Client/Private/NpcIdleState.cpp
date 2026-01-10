#include "pch.h"
#include "NpcIdleState.h"

#include "Npc.h"

CNpcIdleState::CNpcIdleState() : CState()
{
}

HRESULT CNpcIdleState::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CNpcIdleState::Start(void* pArg, CState* pPreState)
{
    CNpc* pNpc = static_cast<CNpc*>(m_pOwner);

    if(pNpc->Get_NpcDesc()->iNpcID == 1)
        pNpc->Set_Animation("MV_Nikke_ScarletVolt_1stMeet_NA_961_03_05_loop");
    else
    {
        m_szAnimationName = pNpc->Get_NpcDesc()->szAnimationName;
        m_szAnimationName += "_Idle";
        pNpc->Set_Animation(m_szAnimationName.c_str());
    }
}

void CNpcIdleState::Update(_float fTimeDelta)
{
    CNpc* pNpc = static_cast<CNpc*>(m_pOwner);
    pNpc->Play_Animation(fTimeDelta);
}

void CNpcIdleState::End()
{
}

CNpcIdleState* CNpcIdleState::Create(void* pArg)
{
    CNpcIdleState* pNpcIdleState = new CNpcIdleState();
    if (FAILED(pNpcIdleState->Initialize(pArg)))
    {
        Safe_Release(pNpcIdleState);
        MSG_BOX("Create Fail : Npc Idle State");
    }
    return pNpcIdleState;
}

void CNpcIdleState::Free()
{
    __super::Free();
}
