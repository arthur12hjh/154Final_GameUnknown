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
    m_Desc->isLockChangable = false;
    m_Desc->isUsingRepulse = true;

    m_eState = PLAYER_STATE::REPULSE;
    m_eAnimState = REPULSE_STATE::ATTACK_START;

    m_pPlayer->Set_Animation("P_Eve_Sword_Normal_MoveBackAttack1_S", false, 1.4f);
    m_pGameInstance->Active_RadialBlur(2.f, 16, 0.5f);
    m_pGameInstance->Active_DoF(true, 0.3f);
    
    _float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) - 
        m_Desc->pPlayerTransform->Get_State(STATE::POSITION)));

    m_pGameInstance->Set_DoFInfo(fDist, 2.f, 1.f);
    m_pGameInstance->SetGameSpeed(0.2f);
    m_pPlayer->SetSkillDataID(2001);
}

PLAYER_TRANSITION_DESC CPlayer_RepulseState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = {};
    //ratio 받아와서 세팅
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    if (REPULSE_STATE::ATTACK_FLOAT == m_eAnimState && (63.f / 119.f) <= fAnimationRatio)
        isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta * 1.7f);
    else
        isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

 /*   if (REPULSE_STATE::ATTACK_FLOAT == m_eAnimState && true == isAnimFinished)
        m_tNextState.eNextState = PLAYER_STATE::IDLE;*/

    if (REPULSE_STATE::ATTACK_FLOAT == m_eAnimState && true == isAnimFinished)
        m_tNextState.eNextState = PLAYER_STATE::IDLE;

    _float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) -
        m_Desc->pPlayerTransform->Get_State(STATE::POSITION)));

    m_pGameInstance->Set_DoFInfo(fDist, 2.f, 1.f);


    //수그렸다가 썸머솔트킥으로 넘어가는 로직
    if (REPULSE_STATE::ATTACK_START == m_eAnimState && true == isAnimFinished)
    {
        m_eAnimState = REPULSE_STATE::ATTACK_FLOAT;
        m_pPlayer->Set_Animation("P_Eve_Sword_Normal_MoveBackAttack1_E", false, 1.6f, 0.f);
        m_pGameInstance->Active_DoF(false, 0.2f);
        m_Desc->pPlayerController->Set_Gravity(true, 14.f);
        m_Desc->isUsingRepulse = false;
    }
    
    if(REPULSE_STATE::ATTACK_FLOAT == m_eAnimState && 0.4f >= fAnimationRatio && 
        true == m_Desc->pPlayerController->Get_Gravity())
        m_Desc->pPlayerTransform->Go_Backward(fTimeDelta * 1.8f);
    if(REPULSE_STATE::ATTACK_FLOAT == m_eAnimState && 0.3f < fAnimationRatio)
        m_pGameInstance->SetGameSpeed(1.f);

    if (REPULSE_STATE::ATTACK_FLOAT == m_eAnimState && 0.8f < fAnimationRatio)
    {
        // 움직이려고 하면 캔슬해서 걷기
        if ((m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
            m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
            m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
            m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)))
        {
            m_tNextState.eNextState = PLAYER_STATE::WALK;

            if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
                m_tNextState.eNextState = PLAYER_STATE::EVADE;
        }
        if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E))
            m_tNextState.eNextState = PLAYER_STATE::PARRY;
    }

    return m_tNextState;
}

_float CPlayer_RepulseState::End()
{
    CGameInstance::GetInstance()->Active_DoF(false, 0.f);
    m_pGameInstance->SetGameSpeed(1.f);
    m_Desc->isInvincible = false;
    m_Desc->isLockChangable = true;
    m_pPlayer->SetSkillDataID(-1);

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
