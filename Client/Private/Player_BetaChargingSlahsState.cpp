#include "pch.h"

#include "Player_BetaChargingSlahsState.h"

#include "Player.h"
#include "GameInstance.h"


CPlayer_BetaChargingSlahsState::CPlayer_BetaChargingSlahsState()
    : CPlayerState {}
{
}

void CPlayer_BetaChargingSlahsState::Start(void* pArg)
{
    m_eState = PLAYER_STATE::BETA_CHARGINGSLASH;
    m_Desc->isLookFixed = true;

    m_pPlayer->Set_Animation("P_Eve_Sword_Beta_ChargeSlash1_Ex_Charging", false, 1.2f);
    m_isStartCharge = true;
}

PLAYER_TRANSITION_DESC CPlayer_BetaChargingSlahsState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 1.f);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    if (true == isAnimFinished && true == m_isStartCharge)
    {
        m_pPlayer->Set_Animation("P_Eve_Sword_Beta_ChargeSlash1_Ex_ChargingLoop", true, 1.2f);
        m_isLoopCharge = true;
        m_isStartCharge = false;
    }

    if (false == m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_2) && true == m_isStartCharge)
    {
        m_tNextState.eNextState = PLAYER_STATE::IDLE;
    }
    else if (false == m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_2) &&  true == m_isLoopCharge)
    {
        // �ϴ� ���÷� �־�а̴ϴ�.
        m_pPlayer->Set_ImpactForce(10.f * fAnimationRatio);
        m_pPlayer->Set_Animation("P_Eve_Sword_Beta_ChargeSlash1_Ex", false, 1.5f);

        m_isLoopCharge = false;
        m_isAttack = true;
    }
     
    if (true == m_isAttack && 
        (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)) && fAnimationRatio > 0.65f)
        m_tNextState.eNextState = PLAYER_STATE::WALK;


    if (true == m_isAttack && true == isAnimFinished)
    {
        m_pPlayer->Set_ImpactForce(0.f);
        m_tNextState.eNextState = PLAYER_STATE::IDLE;
    }

    return m_tNextState;
}

void CPlayer_BetaChargingSlahsState::End()
{
    m_Desc->isLookFixed = false;
}

CPlayer_BetaChargingSlahsState* CPlayer_BetaChargingSlahsState::Create(void* pArg)
{
    return new CPlayer_BetaChargingSlahsState();
}

void CPlayer_BetaChargingSlahsState::Free()
{
    __super::Free();
}
