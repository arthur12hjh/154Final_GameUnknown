#include "pch.h"

#include "Player_IdleState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_IdleState::CPlayer_IdleState() 
    : CPlayerState {}
{
}

void CPlayer_IdleState::Start(void* pArg)
{
    m_eState = PLAYER_STATE::IDLE;
    m_pPlayer->Set_Animation("Proto_Idle", true, 1.2f);
}

PLAYER_TRANSITION_DESC CPlayer_IdleState::Update(_float fTimeDelta)
{
    m_pPlayer->Play_Animation(fTimeDelta);

    if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
    {
        m_tNextState.eNextState = PLAYER_STATE::LIGHT_ATTACK;
        m_tNextState.isChangeMode = true;
        m_tNextState.eMode = PLAYER_MODE::BATTLE;
    }

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
        m_tNextState.eNextState = PLAYER_STATE::EVADE;

    else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
        m_tNextState.eNextState = PLAYER_STATE::WALK;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
    {
        m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;
        m_tNextState.isChangeMode = true;
        m_tNextState.eMode = PLAYER_MODE::BATTLE;
    }

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
    {
        m_tNextState.eNextState = PLAYER_STATE::JUMP;
        m_tNextState.isChangeMode = true;
        m_tNextState.eMode = PLAYER_MODE::BATTLE;
    }

    return m_tNextState;
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
