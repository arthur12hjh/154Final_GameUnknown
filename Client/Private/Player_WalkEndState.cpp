#include "pch.h"
#include "Player_WalkEndState.h"

#include "Player.h"
#include "GameInstance.h"

#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_WalkState.h"
#pragma endregion

CPlayer_WalkEndState::CPlayer_WalkEndState()
    : CPlayerState{}
{
}

void CPlayer_WalkEndState::Start(void* pArg)
{
    m_pPlayer->Set_Animation("Proto_Battle_Run_End", false);
}

CPlayerState* CPlayer_WalkEndState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_W) ||
        m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_A) ||
        m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_S) ||
        m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_D))
    {
        m_pNextState = CPlayer_WalkState::Create(nullptr);
    }

    else if (true == isAnimFinished)
        m_pNextState = CPlayer_IdleState::Create(nullptr);


    m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * max((1 - fAnimationRatio * 3.f), 0.f), nullptr);

    return m_pNextState;
}

void CPlayer_WalkEndState::End()
{
}

CPlayer_WalkEndState* CPlayer_WalkEndState::Create(void* pArg)
{
    return new CPlayer_WalkEndState();
}

void CPlayer_WalkEndState::Free()
{
    __super::Free();
}
