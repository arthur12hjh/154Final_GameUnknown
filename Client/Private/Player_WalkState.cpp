#include "pch.h"
#include "Player_WalkState.h"

#include "Player.h"
#include "GameInstance.h"

#pragma region TRANSFER_STATE

#include "Player_WalkEndState.h"
#include "Player_JumpState.h"
#include "Player_LightAttackState.h"
#include "Player_BetaChargingSlahsState.h"
#include "Player_EvadeState.h"
#pragma endregion

CPlayer_WalkState::CPlayer_WalkState(_bool isLanding)
    : CPlayerState{}
    , m_isLanding { isLanding }
{
}

void CPlayer_WalkState::Start(void* pArg)
{
    //그냥 뛰어
    if(true == m_isLanding)
        m_pPlayer->Set_Animation("Proto_Jump_Run", false);
    else
        m_pPlayer->Set_Animation("Proto_Battle_Run_Start", false);

    m_isRunStart = true;
}

CPlayerState* CPlayer_WalkState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

    _vector vCameraLook = XMVector3Normalize(XMVectorSetY(XMLoadFloat4(m_pGameInstance->Get_CamLook()), 0.f));
    _bool isWalking = { false };

    m_fDegree = 0.f;

    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
    {
        isWalking = true;

        if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
            m_fDegree -= 90.f;

        else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
            m_fDegree += 90.f;
    }

    else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
    {
        m_fDegree += 180.f;
        isWalking = true;

        if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
            m_fDegree += 90.f;

        else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
            m_fDegree -= 90.f;
    }

    else if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) || m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
    {
        if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
        {
            m_fDegree -= 90.f;

            isWalking = true;
        }

        if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
        {
            m_fDegree += 90.f;
            isWalking = true;
        }
    }

    _matrix matRot = XMMatrixRotationY(XMConvertToRadians(m_fDegree));
    vCameraLook = XMVector3Normalize(XMVectorSetY(XMVector3TransformNormal(vCameraLook, matRot), 0.f));

    // A나 D를 누르면 카메라 벡터를 기준으로 좌우로 움직이게끔 세팅.


    _vector vPlayerLook = XMVector3Normalize(XMVectorSetY(m_Desc->pPlayerTransform->Get_State(STATE::LOOK), 0.f));
    _vector vResult = XMVectorLerp(vPlayerLook, vCameraLook, 0.3f);
    m_Desc->pPlayerTransform->Change_Look(vResult);


    //내적 연산결과
    _float fDot = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vPlayerLook, vCameraLook))));

    if (fDot >= 100.f && false == m_isChangingDir && false == m_isRunStart)
    {
        m_isChangingDir = true;
        m_pPlayer->Set_Animation("Proto_Battle_Run_Start", false);
    }
     
    if ((true == m_isRunStart && true == isAnimFinished) || (true == m_isChangingDir && fDot <= 5.f))
    {
        m_pPlayer->Set_Animation("Proto_Battle_Run", true);
        m_isRunStart = false;
        m_isChangingDir = false;
    }

    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
        m_pNextState = CPlayer_JumpState::Create(nullptr);

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
        m_pNextState = CPlayer_EvadeState::Create(nullptr);

    else if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
        m_pNextState = CPlayer_LightAttackState::Create(nullptr);

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
        m_pNextState = CPlayer_BetaChargingSlahsState::Create(nullptr);

    else if (false == isWalking)
        m_pNextState = CPlayer_WalkEndState::Create(nullptr);
    // 이동은 제일 마지막에.
    m_Desc->pPlayerTransform->Go_Straight(fTimeDelta);

    return m_pNextState;
}

void CPlayer_WalkState::End()
{
}

CPlayer_WalkState* CPlayer_WalkState::Create(void* pArg)
{
    _bool isLanding = false;

    if(nullptr != pArg)
        isLanding = static_cast<_bool*>(pArg);

    return new CPlayer_WalkState(isLanding);
}

void CPlayer_WalkState::Free()
{
    __super::Free();
}
