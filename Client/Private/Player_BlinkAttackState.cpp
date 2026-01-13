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
    m_pPlayer->Set_Animation("P_Eve_Sword_Normal_FlashBehindAttack_E", false, 2.f, 0.12f, false, -1.f, 0.f, true, false);
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
        XMVectorSetY(vTargetPos + XMVector3Normalize(vPlayerToMonster) * fRadius * 2.f, XMVectorGetY(vPlayerPos)));
    m_Desc->pPlayerController->Set_Position(XMVectorSetY(vTargetPos + XMVector3Normalize(vPlayerToMonster) * fRadius * 2.f, XMVectorGetY(vPlayerPos)));

    m_Desc->pPlayerTransform->LookAt(XMVectorSetY(vTargetPos, 
        XMVectorGetY(m_Desc->pPlayerTransform->Get_State(STATE::POSITION))));
}

PLAYER_TRANSITION_DESC CPlayer_BlinkAttackState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    //ratio 받아와서 세팅
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    m_fLerp += fAnimationRatio;

    if (m_fLerp >= 1.f)
        m_fLerp = 1.f;

    m_pGameInstance->SetGameSpeed(m_fLerp);


    if (0.8f < fAnimationRatio)
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

    if (true == isAnimFinished)
    {
        m_tNextState.eNextState = PLAYER_STATE::IDLE;
        m_pGameInstance->Active_DoF(false, 0.2f);
    }

    return m_tNextState;
}

_float CPlayer_BlinkAttackState::End()
{
    CGameInstance::GetInstance()->Active_DoF(false, 0.f);
    m_Desc->pPlayerController->Set_CCTCollision(true);
    m_Desc->pPlayerController->Set_Active(true);
    m_Desc->isInvincible = false;
    m_Desc->isLockChangable = true;
    m_Desc->isUsingBlink = false;
    m_pPlayer->SetSkillDataID(-1);

    CGameInstance::GetInstance()->Set_MoitonBlur_Active(false);

    CGameInstance::GetInstance()->ADD_FrameFinalFunction([&]() {
        CGameInstance::GetInstance()->Set_MoitonBlur_Active(true);
        }, 4);

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
