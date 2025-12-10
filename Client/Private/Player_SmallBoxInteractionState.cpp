#include "pch.h"

#include "Player_SmallBoxInteractionState.h"

#include "Interaction_Component.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_SmallBoxInteractionState::CPlayer_SmallBoxInteractionState(void* pArg)
{
    m_pInteractionCom = static_cast<CInteraction_Component*>(pArg);
}

void CPlayer_SmallBoxInteractionState::Start(void* pArg, _float fBlendRatio)
{
    m_eState = PLAYER_STATE::SMALLBOX_INTERACTION;
    m_pPlayer->Set_Animation("Proto_Idle", true, 1.2f);

    CGameObject* pTarget = m_pInteractionCom->GetOwner();

    _vector vPlayerPos = m_pPlayer->GetTransform()->Get_State(STATE::POSITION);
    _vector vTargetPos = pTarget->GetTransform()->Get_State(STATE::POSITION);
    _vector vTargetLook = pTarget->GetTransform()->Get_State(STATE::LOOK);
    //x z만 타겟 따라감
    // 타겟의 위치 + 타겟의 룩 * 3 방향으로 방향 세팅
    m_pPlayer->GetTransform()->Set_State(STATE::POSITION, XMVectorSetY(vTargetLook, 0.f) * 3.f + XMVectorSetY(vTargetPos, XMVectorGetY(vPlayerPos)));
}

PLAYER_TRANSITION_DESC CPlayer_SmallBoxInteractionState::Update(_float fTimeDelta)
{
    if (true == m_isLerpFinished)
    {
        _bool isAnimationFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 1.f);

        if (true == isAnimationFinished)
        {
            m_tNextState.eNextState = PLAYER_STATE::IDLE;
            //상호작용 스타트
            m_pInteractionCom->Action_InteractionEvent(m_pPlayer);
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
        if (1.f >= XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vPlayerLook, XMVector3Normalize(vTargetPos - vPlayerPos))))))

        {
            m_isLerpFinished = true;
            m_pPlayer->Set_Animation("P_Eve_Interaction_SupplyBox_Normal", false, 1.2f);
        }
    }

    return m_tNextState;
}

_float CPlayer_SmallBoxInteractionState::End()
{
    return m_fNextBlendRatio;
}

CPlayer_SmallBoxInteractionState* CPlayer_SmallBoxInteractionState::Create(void* pArg)
{
    return new CPlayer_SmallBoxInteractionState(pArg);
}

void CPlayer_SmallBoxInteractionState::Free()
{
    __super::Free();
}
