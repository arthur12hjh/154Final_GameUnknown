#include "pch.h"
#include "Player_VendingInteractionState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_VendingInteractionState::CPlayer_VendingInteractionState()
    : CPlayerState {}
{
}

void CPlayer_VendingInteractionState::Start(void* pArg)
{
    m_eState = PLAYER_STATE::VENDING_INTERACTION;

    m_pPlayer->Set_Animation("P_Eve_Interaction_VendingMachine", false, 1.2f);
}

PLAYER_TRANSITION_DESC CPlayer_VendingInteractionState::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    _bool isAnimationFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 1.f);

    if (true == isAnimationFinished)
    {
        m_tNextState.eNextState = PLAYER_STATE::IDLE;
    }

    return m_tNextState;
}

void CPlayer_VendingInteractionState::End()
{
}

CPlayer_VendingInteractionState* CPlayer_VendingInteractionState::Create(void* pArg)
{
    return new CPlayer_VendingInteractionState();
}

void CPlayer_VendingInteractionState::Free()
{
    __super::Free();
}
