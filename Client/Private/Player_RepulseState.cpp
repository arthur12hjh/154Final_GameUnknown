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
    m_eState = PLAYER_STATE::REPULSE;
    m_pPlayer->Set_Animation("P_Eve_Sword_SlotNormal_ShieldBreak2", true, 1.2f);
}

PLAYER_TRANSITION_DESC CPlayer_RepulseState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    //ratio 받아와서 세팅
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    if (true == isAnimFinished)
        m_tNextState.eNextState = PLAYER_STATE::IDLE;

    return m_tNextState;
}

_float CPlayer_RepulseState::End()
{
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
