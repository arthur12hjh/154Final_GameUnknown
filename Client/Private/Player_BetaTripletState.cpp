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

    m_pPlayer->Set_Animation("P_Eve_Sword_SlotNormal_ChainStab", false, 2.f);
    m_pPlayer->Set_ImpactForce(2.f);
}

PLAYER_TRANSITION_DESC CPlayer_BetaTripletState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 1.f);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();



    if (0.33f <= fAnimationRatio && 0.36f > fAnimationRatio)
    {
        m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 10.f * fAnimationRatio);
    }


    if (0.65f <= fAnimationRatio)
    {
        if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
            m_tNextState.eNextState = PLAYER_STATE::EVADE;
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
