#include "pch.h"
#include "Player_LockonIdleState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_LockonIdleState::CPlayer_LockonIdleState()
    : CPlayerState {}
{
}

void CPlayer_LockonIdleState::Start(void* pArg)
{
    m_eState = PLAYER_STATE::IDLE;

    m_pPlayer->Set_Animation("Proto_Lockon_Battle_Idle", true);
}

PLAYER_TRANSITION_DESC CPlayer_LockonIdleState::Update(_float fTimeDelta)
{
    m_pPlayer->Play_Animation(fTimeDelta);

    if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
        m_tNextState.eNextState = PLAYER_STATE::LIGHT_ATTACK;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
        m_tNextState.eNextState = PLAYER_STATE::EVADE;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_W) ||
        m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_A) ||
        m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_S) ||
        m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_D))
        m_tNextState.eNextState = PLAYER_STATE::WALK;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
        m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
        m_tNextState.eNextState = PLAYER_STATE::JUMP;


    return m_tNextState;
}

void CPlayer_LockonIdleState::End()
{
}

CPlayer_LockonIdleState* CPlayer_LockonIdleState::Create(void* pArg)
{
    return new CPlayer_LockonIdleState();
}

void CPlayer_LockonIdleState::Free()
{
    __super::Free();
}
