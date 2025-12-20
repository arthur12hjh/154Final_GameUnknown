#include "pch.h"

#include "Player_IdleWalkEndState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_IdleWalkEndState::CPlayer_IdleWalkEndState()
    : CPlayerState{}
{
}

void CPlayer_IdleWalkEndState::Start(void* pArg, _float fBlendRatio)
{
    m_eState = PLAYER_STATE::WALK_END;

    m_pPlayer->Set_Animation("Proto_Walk_END_L", false, 1.f, 0.25);
}

PLAYER_TRANSITION_DESC CPlayer_IdleWalkEndState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 1.f);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    if ((m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_W) ||
        m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_A) ||
        m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_S) ||
        m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_D)) &&
        fAnimationRatio >= 0.1f)
    {
        m_tNextState.eNextState = PLAYER_STATE::WALK;
    }
    else if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)) &&
        fAnimationRatio >= 0.1f)
    {
        m_tNextState.isChangeMode = true;
        m_tNextState.eMode = PLAYER_MODE::BATTLE;
        m_tNextState.eNextState = PLAYER_STATE::LIGHT_ATTACK;
    }

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2) &&
        fAnimationRatio >= 0.1f)
        m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
        m_tNextState.eNextState = PLAYER_STATE::EVADE;

    else if (true == isAnimFinished)
        m_tNextState.eNextState = PLAYER_STATE::IDLE;

    m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * max((1 - fAnimationRatio * 12.f), 0.f), nullptr);

    if(fAnimationRatio >= 0.2f && fAnimationRatio <= 0.45f)
        m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * fAnimationRatio * 0.15f, nullptr);

    return m_tNextState;
}

_float CPlayer_IdleWalkEndState::End()
{
    return m_fNextBlendRatio;
}

CPlayer_IdleWalkEndState* CPlayer_IdleWalkEndState::Create(void* pArg)
{
	return new CPlayer_IdleWalkEndState();
}

void CPlayer_IdleWalkEndState::Free()
{
    __super::Free();
}
