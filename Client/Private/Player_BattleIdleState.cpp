#include "pch.h"
#include "Player_BattleIdleState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_BattleIdleState::CPlayer_BattleIdleState() 
    : CPlayerState {}
{
}

void CPlayer_BattleIdleState::Start(void* pArg)
{
    m_eState = PLAYER_STATE::IDLE;
    m_pPlayer->Set_Animation("Proto_Battle_Idle", true, 1.f);
}

PLAYER_TRANSITION_DESC CPlayer_BattleIdleState::Update(_float fTimeDelta)
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

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_1))
        m_tNextState.eNextState = PLAYER_STATE::BETA_TRIPLET;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
        m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
        m_tNextState.eNextState = PLAYER_STATE::JUMP;

    return m_tNextState;
}

void CPlayer_BattleIdleState::End()
{
}

CPlayer_BattleIdleState* CPlayer_BattleIdleState::Create(void* pArg)
{
    return new CPlayer_BattleIdleState();
}

void CPlayer_BattleIdleState::Free()
{
    __super::Free();
}
