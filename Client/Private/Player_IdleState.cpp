#include "pch.h"
#include "Player_IdleState.h"

#include "Player.h"
#include "GameInstance.h"

#pragma region TRANSFER_STATE

#include "Player_WalkState.h"
#include "Player_JumpState.h"
#include "Player_LightAttackState.h"
#include "Player_BetaChargingSlahsState.h"
#include "Player_EvadeState.h"

#pragma endregion

CPlayer_IdleState::CPlayer_IdleState() 
    : CPlayerState {}
{
}

void CPlayer_IdleState::Start(void* pArg)
{
    m_pPlayer->Set_Animation("Proto_Battle_Idle", true);
}

CPlayerState* CPlayer_IdleState::Update(_float fTimeDelta)
{
    m_pPlayer->Play_Animation(fTimeDelta);

    if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
        m_pNextState = CPlayer_LightAttackState::Create(nullptr);

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
        m_pNextState = CPlayer_EvadeState::Create(nullptr);

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_W) ||
        m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_A) || 
        m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_S) ||
        m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_D))
        m_pNextState = CPlayer_WalkState::Create(nullptr);

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
        m_pNextState = CPlayer_BetaChargingSlahsState::Create(nullptr);

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
        m_pNextState = CPlayer_JumpState::Create(nullptr);

    return m_pNextState;
}

void CPlayer_IdleState::End()
{
}

CPlayer_IdleState* CPlayer_IdleState::Create(void* pArg)
{
    return new CPlayer_IdleState();
}

void CPlayer_IdleState::Free()
{
    __super::Free();
}
