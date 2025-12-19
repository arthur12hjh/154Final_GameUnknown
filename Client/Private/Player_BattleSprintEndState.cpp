#include "pch.h"
#include "Player_BattleSprintEndState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_BattleSprintEndState::CPlayer_BattleSprintEndState()
    : CPlayerState{}
{
}

void CPlayer_BattleSprintEndState::Start(void* pArg, _float fBlendRatio)
{
    m_eState = PLAYER_STATE::SPRINT_END;

    m_pPlayer->Set_Animation("Proto_Battle_Sprint_End", false);
}

PLAYER_TRANSITION_DESC CPlayer_BattleSprintEndState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
    {
        m_tNextState.eNextState = PLAYER_STATE::WALK;
    }

    else if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
        m_tNextState.eNextState = PLAYER_STATE::LIGHT_ATTACK;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_1))
        m_tNextState.eNextState = PLAYER_STATE::BETA_TRIPLET;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
        m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
        m_tNextState.eNextState = PLAYER_STATE::EVADE;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_E) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E))
        m_tNextState.eNextState = PLAYER_STATE::PARRY;

    else if (true == isAnimFinished)
        m_tNextState.eNextState = PLAYER_STATE::IDLE;

    m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * max((1 - fAnimationRatio * 10.f), 0.f), nullptr);

    return m_tNextState;
}

_float CPlayer_BattleSprintEndState::End()
{
    return m_fNextBlendRatio;
}

CPlayer_BattleSprintEndState* CPlayer_BattleSprintEndState::Create(void* pArg)
{
    return new CPlayer_BattleSprintEndState();
}

void CPlayer_BattleSprintEndState::Free()
{
    __super::Free();
}
