#include "pch.h"

#include "Player_BetaChargingSlahsState.h"

#include "Player.h"
#include "GameInstance.h"

#pragma region TRANSFER_STATE

#include "Player_WalkState.h"
#include "Player_IdleState.h"

#pragma endregion

CPlayer_BetaChargingSlahsState::CPlayer_BetaChargingSlahsState()
    : CPlayerState {}
{
}

void CPlayer_BetaChargingSlahsState::Start(void* pArg)
{
    m_pPlayer->Set_Animation("P_Eve_Sword_Beta_ChargeSlash1_Ex_Charging", false);
    m_isStartCharge = true;
}

CPlayerState* CPlayer_BetaChargingSlahsState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 1.f);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();
    //차징이 끝났다면,
    if (true == isAnimFinished && true == m_isStartCharge)
    {
        m_pPlayer->Set_Animation("P_Eve_Sword_Beta_ChargeSlash1_Ex_ChargingLoop", true);
        m_isLoopCharge = true;
        m_isStartCharge = false;
    }

    if (false == m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_2) && true == m_isStartCharge)
    {
        m_pNextState = CPlayer_IdleState::Create(nullptr);
    }
    else if (false == m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_2) &&  true == m_isLoopCharge)
    {
        //애니메이션 재생하고 공격 나가게 처리.
        m_pPlayer->Set_Animation("P_Eve_Sword_Beta_ChargeSlash1_Ex", false);
        m_isLoopCharge = false;
        m_isAttack = true;
    }
     
    if (true == m_isAttack && 
        (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)) && fAnimationRatio > 0.65f)
        m_pNextState = CPlayer_WalkState::Create(nullptr);


    if (true == m_isAttack && true == isAnimFinished)
    {
        m_pNextState = CPlayer_IdleState::Create(nullptr);
    }

    return m_pNextState;
}

void CPlayer_BetaChargingSlahsState::End()
{
}

CPlayer_BetaChargingSlahsState* CPlayer_BetaChargingSlahsState::Create(void* pArg)
{
    return new CPlayer_BetaChargingSlahsState();
}

void CPlayer_BetaChargingSlahsState::Free()
{
    __super::Free();
}
