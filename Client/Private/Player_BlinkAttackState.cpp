#include "pch.h"

#include "Player_BlinkAttackState.h"

#include "Player.h"
#include "Nayitba.h"
#include "CharacterController.h"
#include "GameInstance.h"
#include "GameManager.h"

CPlayer_BlinkAttackState::CPlayer_BlinkAttackState()
    : CPlayerState{}
{
}

//돌면서 크게 한번 베는 공격. ( + 림라이트도 )
void CPlayer_BlinkAttackState::Start(void* pArg, _float fBlendRatio)
{
    //m_pGameInstance->Active_RadialBlur(0.2f, 0.1f, 1.f);
    //m_Desc->pPlayerController->Set_CCTCollision(false);
    m_Desc->isInvincible = true;
    m_Desc->isLockChangable = false;

    m_eState = PLAYER_STATE::BLINK_ATTACK;
    m_pPlayer->Set_Animation("P_Eve_Sword_Normal_FlashBehindAttack_E", false, 1.4f, 0.12f, false, -1.f, 0.f, true, false);
    m_pPlayer->SetSkillDataID(2000);

    CTransform* pTargetTransform = m_pGameManager->Get_TargetTransform();
    
    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
    _vector vTargetLook = pTargetTransform->Get_State(STATE::LOOK);
    _vector vPlayerPos = m_Desc->pPlayerTransform->Get_State(STATE::POSITION);

    //플레이어에서 몬스터로 향하는 벡터
    _vector vPlayerToMonster = vTargetPos - vPlayerPos;

    CCharacterController* pTargetCCT = m_pGameManager->Get_LockonTarget()->Get_CCT();
    _float fRadius = pTargetCCT->Get_Radius();

    //락온 중이니까 lock은 계속 들어가지.
    //- XMVector3Normalize(vTargetLook) * fRadius
    m_Desc->pPlayerTransform->Set_State(STATE::POSITION, 
        XMVectorSetY(vTargetPos + XMVector3Normalize(vPlayerToMonster) * fRadius * 4.f, XMVectorGetY(vPlayerPos)));
    m_Desc->pPlayerController->Set_Position(XMVectorSetY(vTargetPos + XMVector3Normalize(vPlayerToMonster) * fRadius * 4.f, XMVectorGetY(vPlayerPos)));

    m_Desc->pPlayerTransform->LookAt(XMVectorSetY(vTargetPos, 
        XMVectorGetY(m_Desc->pPlayerTransform->Get_State(STATE::POSITION))));
}

PLAYER_TRANSITION_DESC CPlayer_BlinkAttackState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    //ratio 받아와서 세팅
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    if (true == isAnimFinished)
    {
        m_tNextState.eNextState = PLAYER_STATE::IDLE;
        m_pGameInstance->Active_DoF(false, 0.2f);
        m_pGameInstance->SetGameSpeed(1.f);
    }

    return m_tNextState;
}

_float CPlayer_BlinkAttackState::End()
{
    m_Desc->pPlayerController->Set_CCTCollision(true);
    m_Desc->pPlayerController->Set_Active(true);
    m_Desc->isInvincible = false;
    m_Desc->isLockChangable = true;
    m_pPlayer->SetSkillDataID(-1);

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
