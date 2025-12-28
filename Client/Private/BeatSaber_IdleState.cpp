#include "pch.h"
#include "BeatSaber_IdleState.h"

#include "BeatSaberCharacter.h"

CBeatSaber_IdleState::CBeatSaber_IdleState() : CState()
{
}

HRESULT CBeatSaber_IdleState::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CBeatSaber_IdleState::Start(void* pArg, CState* pPreState)
{
    auto pBeatSaberCharacter = static_cast<CBeatSaberCharacter*>(m_pOwner);
    pBeatSaberCharacter->Set_Animation("N_Dororong_Idle");
}

void CBeatSaber_IdleState::Update(_float fTimeDelta)
{
    auto BeatSaberCharacter = static_cast<CBeatSaberCharacter*>(m_pOwner);
    BeatSaberCharacter->Play_Animation(fTimeDelta);
}

void CBeatSaber_IdleState::End()
{
}

CBeatSaber_IdleState* CBeatSaber_IdleState::Create(void* pArg)
{
    CBeatSaber_IdleState* pBeatSaber_Idle = new CBeatSaber_IdleState();
    if (FAILED(pBeatSaber_Idle->Initialize(pArg)))
    {
        Safe_Release(pBeatSaber_Idle);
        MSG_BOX("Create Fail : BeatSaber Idle");
    }
    return pBeatSaber_Idle;
}

void CBeatSaber_IdleState::Free()
{
    __super::Free();
}
