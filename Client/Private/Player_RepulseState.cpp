#include "pch.h"

#include "Player_RepulseState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_RepulseState::CPlayer_RepulseState()
    : CPlayerState {}
{
}

//리펄스는 시작부터 끝까지 묶어서 작성. ( 포지션은 럴프 처리해야됨 )
void CPlayer_RepulseState::Start(void* pArg, _float fBlendRatio)
{
    m_Desc->isInvincible = true;
    m_eState = PLAYER_STATE::REPULSE;
    m_eAnimState = REPULSE_STATE::ATTACK_START;

    m_pPlayer->Set_Animation("P_Eve_Sword_Beta_ShieldBreak_Charging", false, 1.4f);
    m_pGameInstance->Active_RadialBlur(2.f, 6, 0.3f);
    m_pGameInstance->Active_DoF(true, 0.3f);
    
    _float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) - 
        m_Desc->pPlayerTransform->Get_State(STATE::POSITION)));

    m_pGameInstance->Set_DoFInfo(fDist, 2.f, 1.f);
    m_pGameInstance->SetGameSpeed(0.4f);
}

PLAYER_TRANSITION_DESC CPlayer_RepulseState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    //ratio 받아와서 세팅
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    if (REPULSE_STATE::ATTACK_START == m_eAnimState && ((20.f / 54.f) <= fAnimationRatio))
    {
        m_eAnimState = REPULSE_STATE::ATTACK_FLOAT;
        m_pPlayer->Set_Animation("P_Eve_Sword_SlotNormal_ShieldBreak2", false, 1.1f);
        m_Desc->pPlayerController->Set_Gravity(true, 16.5f);
        m_pGameInstance->Active_DoF(false, 0.2f);
    }

    if(REPULSE_STATE::ATTACK_FLOAT == m_eAnimState && 0.5f >= fAnimationRatio && 0.05f <= fAnimationRatio)
        m_Desc->pPlayerTransform->Go_Backward(fTimeDelta * 1.5f);

    if(REPULSE_STATE::ATTACK_FLOAT == m_eAnimState && 0.5f >= fAnimationRatio)
        m_pGameInstance->SetGameSpeed(1.f);

    if (REPULSE_STATE::ATTACK_FLOAT == m_eAnimState && true == isAnimFinished)
        m_tNextState.eNextState = PLAYER_STATE::IDLE;

    return m_tNextState;
}

_float CPlayer_RepulseState::End()
{
    m_pGameInstance->SetGameSpeed(1.f);
    m_Desc->isInvincible = false;

    return m_fNextBlendRatio;
}

CPlayer_RepulseState* CPlayer_RepulseState::Create(void* pArg)
{
    return new CPlayer_RepulseState();
}

void CPlayer_RepulseState::Free()
{
    __super::Free();
}
