#include "pch.h"
#include "BeatSaber_MoveState.h"

#include "BeatSaberCharacter.h"

CBeatSaber_MoveState::CBeatSaber_MoveState() : CState()
{
}

HRESULT CBeatSaber_MoveState::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CBeatSaber_MoveState::Start(void* pArg, CState* pPreState)
{





}

void CBeatSaber_MoveState::Update(_float fTimeDelta)
{
    auto BeatSaberCharacter = static_cast<CBeatSaberCharacter*>(m_pOwner);


    BeatSaberCharacter->Play_Animation(fTimeDelta);
}

void CBeatSaber_MoveState::End()
{

}

CBeatSaber_MoveState* CBeatSaber_MoveState::Create(void* pArg)
{
    CBeatSaber_MoveState* pBeatSaber_Move = new CBeatSaber_MoveState();
    if (FAILED(pBeatSaber_Move->Initialize(pArg)))
    {
        Safe_Release(pBeatSaber_Move);
        MSG_BOX("Create Fail : BeatSaber Move");
    }
    return pBeatSaber_Move;
}

void CBeatSaber_MoveState::Free()
{
    __super::Free();
}