#include "pch.h"
#include "Player_GrabState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_GrabState::CPlayer_GrabState(CHARACTER_SKILL_DESC* pDesc)
    : CPlayerState{}
{
    // 애니메이션이랑 본 이름 복사해옴
    memcpy(m_szAnimName, pDesc->szHitAnimationName, sizeof(_char) * 256);
    memcpy(m_szTargetBoneName, pDesc->szLinkBoneName, sizeof(_char) * 256);
}

void CPlayer_GrabState::Start(void* pArg, _float fBlendRatio)
{
    m_eState = PLAYER_STATE::GRAB;

    m_pPlayer->Set_Animation(m_szAnimName, true, 1.2f, 0.f);

    // CCT 간의 충돌 꺼주기
    m_Desc->pPlayerController->Set_CCTCollision(false);
    m_Desc->isGrabbed = true;
}

PLAYER_TRANSITION_DESC CPlayer_GrabState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    if (true == isAnimFinished)
        m_tNextState.eNextState = PLAYER_STATE::IDLE;

    return m_tNextState;
}

_float CPlayer_GrabState::End()
{
    m_Desc->pGrabAttackter = nullptr;
    m_Desc->pGrabBone = nullptr;
    m_Desc->pPlayerController->Set_CCTCollision(true);
    m_Desc->isGrabbed = false;

    return m_fNextBlendRatio;
}

CPlayer_GrabState* CPlayer_GrabState::Create(void* pArg)
{
    CHARACTER_SKILL_DESC* Desc = static_cast<CHARACTER_SKILL_DESC*>(pArg);

    return new CPlayer_GrabState(Desc);
}

void CPlayer_GrabState::Free()
{
    __super::Free();
}
