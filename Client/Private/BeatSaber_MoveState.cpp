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

    SelectEvadeAnim(BeatSaberCharacter);
    BeatSaberCharacter->Play_Animation(fTimeDelta);
}

void CBeatSaber_MoveState::End()
{

}

void CBeatSaber_MoveState::SelectEvadeAnim(CBeatSaberCharacter* pCharacter)
{
    auto& pCharacterDesc = pCharacter->GetBeatSaberCharacterDesc();
    switch (pCharacterDesc.eDirection)
    {
    case DIRECTION::LEFT :
        pCharacter->Set_Animation("N_Dororong_Evade", true, 1.f, 0.12f, false, 30.f, 18.f);
        break;

    case DIRECTION::RIGHT:
        pCharacter->Set_Animation("N_Dororong_Evade", true, 1.f, 0.12f, false, 10.f, 0.f);
        break;

    case DIRECTION::LEFT_FRONT:
        pCharacter->Set_Animation("N_Dororong_Evade", true, 1.f, 0.12f, false, 40.f, 38.f);
        break;

    case DIRECTION::RIGHT_FRONT:
        pCharacter->Set_Animation("N_Dororong_Exhaust_Evade", true, 1.f, 0.12f, false, 40.f, 38.f);
        break;

    case DIRECTION::LEFT_BACK:
        pCharacter->Set_Animation("N_Dororong_Exhaust_Evade", true, 1.f, 0.12f, false, 30.f, 18.f);
        break;

    case DIRECTION::RIGHT_BACK:
        pCharacter->Set_Animation("N_Dororong_Exhaust_Evade", true, 1.f, 0.12f, false, 10.f, 0.f);
        break;
    }
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