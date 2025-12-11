#include "pch.h"
#include "Player_LockonSprintEndState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_LockonSprintEndState::CPlayer_LockonSprintEndState(PLAYER_DIRECTION eDir)
    : CPlayerState{}
    , m_eStartDirection { eDir }
{
}

void CPlayer_LockonSprintEndState::Start(void* pArg, _float fBlendRatio)
{
    m_eState = PLAYER_STATE::SPRINT_END;

    switch (m_eStartDirection)
    {
    case PLAYER_DIRECTION::STRAIGHT:
        m_pPlayer->Set_Animation("Proto_Lockon_Sprint_Forward_End", false);
        break;
    case PLAYER_DIRECTION::LEFT:
        m_pPlayer->Set_Animation("Proto_Lockon_Sprint_Left_End", false);
        break;
    case PLAYER_DIRECTION::RIGHT:
        m_pPlayer->Set_Animation("Proto_Lockon_Sprint_Right_End", false);
        break;
    case PLAYER_DIRECTION::BACKWARD:
        m_pPlayer->Set_Animation("Proto_Lockon_Sprint_Backward_End", false);
        break;

    default:
        break;
    }
}

PLAYER_TRANSITION_DESC CPlayer_LockonSprintEndState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D) &&
        0.4f <= fAnimationRatio)
    {
        m_tNextState.eNextState = PLAYER_STATE::WALK;
    }

    else if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)) &&
        0.4f <= fAnimationRatio)
        m_tNextState.eNextState = PLAYER_STATE::LIGHT_ATTACK;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_1) &&
        0.4f <= fAnimationRatio)
        m_tNextState.eNextState = PLAYER_STATE::BETA_TRIPLET;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2) &&
        0.4f <= fAnimationRatio)
        m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT) &&
        0.4f <= fAnimationRatio)
        m_tNextState.eNextState = PLAYER_STATE::EVADE;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_E) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E) && 
        0.4f <= fAnimationRatio)
        m_tNextState.eNextState = PLAYER_STATE::PARRY;

    else if (true == isAnimFinished)
        m_tNextState.eNextState = PLAYER_STATE::IDLE;

    //m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * max((1 - fAnimationRatio * 4.f), 0.f), nullptr);

    return m_tNextState;
}

_float CPlayer_LockonSprintEndState::End()
{
    return m_fNextBlendRatio;
}

CPlayer_LockonSprintEndState* CPlayer_LockonSprintEndState::Create(void* pArg)
{
	PLAYER_DIRECTION eDir = PLAYER_DIRECTION::END;

    if (nullptr != pArg)
    {
        eDir = *static_cast<PLAYER_DIRECTION*>(pArg);
        return new CPlayer_LockonSprintEndState(eDir);
    }

    return new CPlayer_LockonSprintEndState(eDir);
}

void CPlayer_LockonSprintEndState::Free()
{
    __super::Free();
}
