#include "pch.h"

#include "Player_BetaChargingSlashState.h"

#include "Player.h"
#include "GameInstance.h"


CPlayer_BetaChargingSlashState::CPlayer_BetaChargingSlashState()
    : CPlayerState {}
{
}

_bool CPlayer_BetaChargingSlashState::CanEnter(class CPlayer* pPlayer, PLAYER_DESC* pPlayerDesc)
{
    //차징 슬래시는 1004번
    if (true == pPlayer->Use_BetaSkill(1004))
        return true;

    return false;
}

void CPlayer_BetaChargingSlashState::Start(void* pArg, _float fBlendRatio)
{
    m_eState = PLAYER_STATE::BETA_CHARGINGSLASH;
    m_Desc->isLookFixed = true;
    m_Desc->isSuperArmor = true;

    m_pPlayer->Set_Animation("P_Eve_Sword_Beta_ChargeSlash1_Ex_Charging", false, 1.2f);
    m_isStartCharge = true;
}

PLAYER_TRANSITION_DESC CPlayer_BetaChargingSlashState::Update(_float fTimeDelta)
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
        // 플레이어의 충격량.
        m_pPlayer->Set_ImpactForce(10.f * fAnimationRatio);
        m_pPlayer->Set_Animation("P_Eve_Sword_Beta_ChargeSlash1_Ex", false, 1.5f);

        m_isLoopCharge = false;
        m_isAttack = true;
    }
     
    if (true == m_isAttack && 
        (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A)  ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S)  ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)) && fAnimationRatio > 0.5f)
        m_tNextState.eNextState = PLAYER_STATE::WALK;

    else if (true == m_isAttack &&
        (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT) && fAnimationRatio > 0.5f))
        m_tNextState.eNextState = PLAYER_STATE::EVADE;

    else if (true == m_isAttack &&
        (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E) && fAnimationRatio > 0.5f))
        m_tNextState.eNextState = PLAYER_STATE::PARRY;


    if (true == m_isAttack && true == isAnimFinished)
    {
        m_pPlayer->Set_ImpactForce(0.f);
        m_tNextState.eNextState = PLAYER_STATE::IDLE;
    }

    return m_tNextState;
}

_float CPlayer_BetaChargingSlashState::End()
{
    m_Desc->isLookFixed = false;
    m_Desc->isSuperArmor = false;

    return m_fNextBlendRatio;
}

CPlayer_BetaChargingSlashState* CPlayer_BetaChargingSlashState::Create(void* pArg)
{
    return new CPlayer_BetaChargingSlashState();
}

void CPlayer_BetaChargingSlashState::Free()
{
    __super::Free();
}
