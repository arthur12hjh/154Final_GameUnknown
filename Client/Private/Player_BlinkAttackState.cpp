#include "pch.h"

#include "Player_BlinkAttackState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_BlinkAttackState::CPlayer_BlinkAttackState()
    : CPlayerState{}
{
}

//돌면서 크게 한번 베는 공격. ( + 림라이트도 )
void CPlayer_BlinkAttackState::Start(void* pArg, _float fBlendRatio)
{
    m_eState = PLAYER_STATE::BLINK_ATTACK;
    m_pPlayer->Set_Animation("P_Eve_Sword_Normal_FlashBehindAttack_E", true, 1.2f);
}

PLAYER_TRANSITION_DESC CPlayer_BlinkAttackState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    //ratio 받아와서 세팅
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    if (true == isAnimFinished)
        m_tNextState.eNextState = PLAYER_STATE::IDLE;

    return m_tNextState;
}

_float CPlayer_BlinkAttackState::End()
{
    return m_fNextBlendRatio;
}

CPlayer_BlinkAttackState* CPlayer_BlinkAttackState::Create(void* pArg)
{
    return new CPlayer_BlinkAttackState();
}

void CPlayer_BlinkAttackState::Free()
{
    __super::Free();
}
