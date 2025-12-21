#include "pch.h"

#include "Player_BlinkAttackState.h"

#include "Player.h"
#include "GameInstance.h"
#include "GameManager.h"

CPlayer_BlinkAttackState::CPlayer_BlinkAttackState()
    : CPlayerState{}
{
}

//돌면서 크게 한번 베는 공격. ( + 림라이트도 )
void CPlayer_BlinkAttackState::Start(void* pArg, _float fBlendRatio)
{
    m_Desc->isLookFixed = true;
    m_Desc->isInvincible = true;
    m_eState = PLAYER_STATE::BLINK_ATTACK;
    m_pPlayer->Set_Animation("P_Eve_Sword_Normal_FlashBehindAttack_E", false, 1.4f);

    CTransform* pTargetTransform = m_pGameManager->Get_TargetTransform();

    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
    _vector vTargetLook = pTargetTransform->Get_State(STATE::LOOK);
    _vector vPlayerPos = m_Desc->pPlayerTransform->Get_State(STATE::POSITION);

    //락온 중이니까 lock은 계속 들어가지.
    m_Desc->pPlayerTransform->Set_State(STATE::POSITION, 
        vTargetPos - vTargetLook * 5.f);
    m_Desc->pPlayerController->Set_Position(XMVectorSetY(vTargetPos - vTargetLook * 5.f, XMVectorGetY(vPlayerPos)));

    m_Desc->pPlayerTransform->LookAt(XMVectorSetY(vTargetPos, 
        XMVectorGetY(m_Desc->pPlayerTransform->Get_State(STATE::POSITION))));
}

PLAYER_TRANSITION_DESC CPlayer_BlinkAttackState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 1.f);
    //ratio 받아와서 세팅
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    if (true == isAnimFinished)
        m_tNextState.eNextState = PLAYER_STATE::IDLE;

    return m_tNextState;
}

_float CPlayer_BlinkAttackState::End()
{
    m_Desc->isLockChangable = true;
    m_Desc->isLookFixed = false;

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
