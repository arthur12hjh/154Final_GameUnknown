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

    m_pPlayer->Set_Animation(m_szAnimName, false, 1.2f, 0.f);

    // CCT 간의 충돌 꺼주기
    m_Desc->pPlayerController->Set_CCTCollision(false);
    m_Desc->isGrabbed = true;
    m_Desc->isInvincible = true;
    m_Desc->isLookFixed = true;
    m_Desc->pPlayerController->Set_Active(false);
}

PLAYER_TRANSITION_DESC CPlayer_GrabState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    if (nullptr != m_Desc->pGrabAttackter && nullptr != m_Desc->pGrabBone)
    {
        _matrix PlayerWorldMatrix =  XMMatrixRotationZ(XMConvertToRadians(-90.f)) * XMMatrixRotationX(XMConvertToRadians(180.f)) 
            * XMLoadFloat4x4(m_Desc->pGrabBone) * XMLoadFloat4x4(m_Desc->pGrabAttackter->GetTransform()->Get_WorldMatrixPtr());

        _vector vScale, vRot, vTrans;
        XMMatrixDecompose(&vScale, &vRot, &vTrans, PlayerWorldMatrix);

        m_Desc->pPlayerTransform->Set_Rotation(vRot, true);
        m_Desc->pPlayerTransform->Set_State(STATE::POSITION, vTrans);
       
        m_Desc->pPlayerController->Set_Position(m_Desc->pPlayerTransform->Get_State(STATE::POSITION));
    }

    if (true == isAnimFinished && false == m_isStandUp)
    {
        m_isStandUp = true;
        // 일어나는 애니메이션 세팅
        //m_pPlayer->Set_Animation(m_szAnimName, true, 1.2f, 0.f);
        m_pPlayer->Set_Animation("Result_State_DownFaceUp_E", false, 1.5f, 0.f);

        m_Desc->pGrabAttackter = nullptr;
        m_Desc->pGrabBone = nullptr;
        m_Desc->pPlayerController->Set_Active(true);
    }
    else if (true == isAnimFinished && true == m_isStandUp)
    {
        //Idle로 돌아가게끔 처리한다
        m_tNextState.eNextState = PLAYER_STATE::IDLE;
    }

    return m_tNextState;
}

_float CPlayer_GrabState::End()
{
    m_Desc->pPlayerController->Set_CCTCollision(true);
    m_Desc->isGrabbed = false;
    m_Desc->isInvincible = false;
    m_Desc->isLookFixed = false;

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
