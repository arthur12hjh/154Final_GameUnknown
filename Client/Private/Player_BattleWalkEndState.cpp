#include "pch.h"
#include "Player_BattleWalkEndState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_BattleWalkEndState::CPlayer_BattleWalkEndState()
    : CPlayerState{}
{
}

void CPlayer_BattleWalkEndState::Start(void* pArg)
{
    m_eState = PLAYER_STATE::WALK_END;

    m_pPlayer->Set_Animation("Proto_Battle_Run_End", false);
}

PLAYER_TRANSITION_DESC CPlayer_BattleWalkEndState::Update(_float fTimeDelta)
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

    else if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)) &&
        fAnimationRatio >= 0.1f)
        m_tNextState.eNextState = PLAYER_STATE::LIGHT_ATTACK;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2) &&
        fAnimationRatio >= 0.1f)
        m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
        m_tNextState.eNextState = PLAYER_STATE::EVADE;

    else if (true == isAnimFinished)
        m_tNextState.eNextState = PLAYER_STATE::IDLE;

    m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * max((1 - fAnimationRatio * 3.f), 0.f), nullptr);

    return m_tNextState;
}

void CPlayer_BattleWalkEndState::End()
{
}

CPlayer_BattleWalkEndState* CPlayer_BattleWalkEndState::Create(void* pArg)
{
    return new CPlayer_BattleWalkEndState();
}

void CPlayer_BattleWalkEndState::Free()
{
    __super::Free();
}
