#include "pch.h"
#include "Player_VendingInteractionState.h"

#include "Interaction_Component.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_VendingInteractionState::CPlayer_VendingInteractionState(void* pArg)
    : CPlayerState {}
{
    m_pInteractionCom = static_cast<CInteraction_Component*>(pArg);
}

void CPlayer_VendingInteractionState::Start(void* pArg, _float fBlendRatio)
{
    m_Desc->isWeaponVisible = false;
    m_Desc->isInteracting = true;
    m_Desc->isCoinVisible = true;
    m_eState = PLAYER_STATE::VENDING_INTERACTION;

    m_pPlayer->Set_Animation("Proto_Idle", true, 1.2f);

    CGameObject* pTarget = m_pInteractionCom->GetOwner();

    _vector vPlayerPos = m_pPlayer->GetTransform()->Get_State(STATE::POSITION);
    _vector vTargetPos = pTarget->GetTransform()->Get_State(STATE::POSITION);
    _vector vTargetLook = pTarget->GetTransform()->Get_State(STATE::LOOK);
    //x z만 타겟 따라감
    // 타겟의 위치 + 타겟의 룩 * 3 방향으로 방향 세팅`
    m_pPlayer->GetTransform()->Set_State(STATE::POSITION, XMVector3Normalize(XMVectorSetY(vTargetLook, 0.f)) * 3.f + XMVectorSetY(vTargetPos, XMVectorGetY(vPlayerPos)));
    m_Desc->pPlayerController->Set_Position(XMVectorSetY(vTargetLook, 0.f) * 3.f + XMVectorSetY(vTargetPos, XMVectorGetY(vPlayerPos)));
}

PLAYER_TRANSITION_DESC CPlayer_VendingInteractionState::Update(_float fTimeDelta)
{
    if (true == m_isLerpFinished)
    {
        _bool isAnimationFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 1.f);

        if (true == isAnimationFinished)
        {
            m_tNextState.eMode = PLAYER_MODE::IDLE;
            m_tNextState.eNextState = PLAYER_STATE::IDLE;
            m_tNextState.isChangeMode = true;
            //상호작용 스타트
            m_pInteractionCom->Action_InteractionEvent(fTimeDelta, m_pPlayer);
        }

    }
    else
    {
        _bool isAnimationFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 1.f);

        CGameObject* pTarget = m_pInteractionCom->GetOwner();
        _vector vTargetPos = pTarget->GetTransform()->Get_State(STATE::POSITION);
        _vector vPlayerPos = m_pPlayer->GetTransform()->Get_State(STATE::POSITION);

        m_pPlayer->GetTransform()->LookAt_Lerp(XMVectorSetY(vTargetPos, XMVectorGetY(vPlayerPos)), 0.1f, 1.f);

        _vector vPlayerLook = m_pPlayer->GetTransform()->Get_State(STATE::LOOK);
        _float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(XMVectorSetY(vPlayerLook, 0.f), XMVector3Normalize(XMVectorSetY(vTargetPos - vPlayerPos, 0.f))))));
        
        if (1.f >= fDegree)
        {
            m_isLerpFinished = true;
            m_pPlayer->Set_Animation("P_Eve_Interaction_VendingMachine", false, 1.2f);
        }
    }

    return m_tNextState;

    //_bool isAnimationFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 1.f);

    //if (true == isAnimationFinished)
    //{
    //    m_pInteractionCom->Action_InteractionEvent(fTimeDelta, m_pPlayer);
    //    m_tNextState.eNextState = PLAYER_STATE::IDLE;
    //}


    //m_pPlayer->Set_Animation("P_Eve_Interaction_VendingMachine", false, 1.2f);

    //return m_tNextState;
}

_float CPlayer_VendingInteractionState::End()
{
    m_Desc->isCoinVisible = false;
    m_Desc->isInteracting = false;

    //m_pInteractionCom->Action_InteractionEvent(m_pGameInstance->Get_TimeDelta(TEXT("GameLoop")), m_pPlayer);

    return m_fNextBlendRatio;
}

CPlayer_VendingInteractionState* CPlayer_VendingInteractionState::Create(void* pArg)
{
    return new CPlayer_VendingInteractionState(pArg);
}

void CPlayer_VendingInteractionState::Free()
{
    __super::Free();
}
