#include "pch.h"

#include "Player_BetaTripletState.h"

#include "Player.h"
#include "GameInstance.h"
#include "GameManager.h"

CPlayer_BetaTripletState::CPlayer_BetaTripletState()
	: CPlayerState{}
{
}

_bool CPlayer_BetaTripletState::CanEnter(CPlayer* pPlayer, PLAYER_DESC* pPlayerDesc)
{
    //Æ®¸®ÇÃ¸´ 1005¹ø
    if (true == pPlayer->Use_BetaSkill(1005))
        return true;

    return false;
}

void CPlayer_BetaTripletState::Start(void* pArg, _float fBlendRatio)
{
    m_eState = PLAYER_STATE::BETA_TRIPLET;

    m_pPlayer->Set_Animation("P_Eve_Sword_SlotNormal_ChainStab", false, 2.5f);
    m_pPlayer->Set_ImpactForce(2.f);
    m_Desc->isSuperArmor = true;
}

PLAYER_TRANSITION_DESC CPlayer_BetaTripletState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 1.f);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();



    if (0.15f <= fAnimationRatio && 0.17f > fAnimationRatio)
    {
        if (false == m_isEffectActed)
        {
            m_pGameInstance->Active_RadialBlur(0.2f, 5, 0.05f);
            m_isEffectActed = true;
        }
    }
    if (0.18f <= fAnimationRatio && 0.20f > fAnimationRatio)
    {
        m_isEffectActed = false;
    }
    if (0.203f <= fAnimationRatio && 0.23f > fAnimationRatio)
    {
        if (false == m_isEffectActed)
        {
            m_pGameInstance->Active_RadialBlur(0.3f, 5, 0.2f);
            m_isEffectActed = true;
        }
    }
    if (0.24f <= fAnimationRatio && 0.26f > fAnimationRatio)
    {
            m_isEffectActed = false;
    }
    
    if (0.33f <= fAnimationRatio && 0.36f > fAnimationRatio)
    {
        m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 10.f * fAnimationRatio);
        
        if (false == m_isEffectActed)
        {
            //m_pGameInstance->Shake_Camera(0.2f, 0.2f);
            m_pGameInstance->Active_RadialBlur(0.5f, 5, 0.6f);
            m_isEffectActed = true;
        }
    }


    if (0.47f <= fAnimationRatio)
    {

        if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
        {
            m_tNextState.eNextState = PLAYER_STATE::LIGHT_ATTACK;
            m_tNextState.isChangeMode = true;
            m_tNextState.eMode = PLAYER_MODE::BATTLE;
        }

        else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E))
            m_tNextState.eNextState = PLAYER_STATE::PARRY;


        else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
            m_tNextState.eNextState = PLAYER_STATE::EVADE;

        else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
            m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
            m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
            m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
            m_tNextState.eNextState = PLAYER_STATE::WALK;

        else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
        {
            m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;
            m_tNextState.isChangeMode = true;
            m_tNextState.eMode = PLAYER_MODE::BATTLE;
        }
    }

    if (true == isAnimFinished)
    {
        m_pPlayer->Set_ImpactForce(0.f);
        m_tNextState.eNextState = PLAYER_STATE::IDLE;
    }

    return m_tNextState;
}

_float CPlayer_BetaTripletState::End()
{
    m_Desc->isSuperArmor = false;
    return m_fNextBlendRatio;
}

CPlayer_BetaTripletState* CPlayer_BetaTripletState::Create(void* pArg)
{
    return new CPlayer_BetaTripletState();
}

void CPlayer_BetaTripletState::Free()
{
    __super::Free();
}
