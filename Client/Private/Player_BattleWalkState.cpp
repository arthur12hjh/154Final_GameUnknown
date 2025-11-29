#include "pch.h"
#include "Player_BattleWalkState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_BattleWalkState::CPlayer_BattleWalkState(_bool isLanding, _bool isEvading)
    : CPlayerState{}
    , m_isLanding { isLanding }
    , m_isEvading { isEvading }
{
}

void CPlayer_BattleWalkState::Start(void* pArg)
{
    m_eState = PLAYER_STATE::WALK;

    m_isRunStart = true;

    //Jump Run Start로 시작.
    if (true == m_isEvading)
    {
        m_isRunStart = false;
        m_pPlayer->Set_Animation("Proto_Battle_Run", true, 1.2f, 0.3f);
    }
    else if (true == m_isLanding)
        m_pPlayer->Set_Animation("Proto_Battle_Jump_Run", false, 1.2f, 0.3f);
    else
        m_pPlayer->Set_Animation("Proto_Battle_Run_Start", false, 1.2f);
}

PLAYER_TRANSITION_DESC CPlayer_BattleWalkState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

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
        m_pPlayer->Set_Animation("Proto_Battle_Run_Start", false, 1.2f);
    }
     
    if ((true == m_isRunStart || true == m_isChangingDir) && fAnimationRatio >= 0.7f) //|| (true == m_isChangingDir && fDot <= 5.f))
    {
        m_pPlayer->Set_Animation("Proto_Battle_Run", true, 1.2f, 0.2f, false);
        m_isRunStart = false;
        m_isChangingDir = false;
    }

    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
        m_tNextState.eNextState = PLAYER_STATE::JUMP;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
        m_tNextState.eNextState = PLAYER_STATE::EVADE;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
        m_tNextState.eNextState = PLAYER_STATE::LIGHT_ATTACK;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
        m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;

    else if (false == isWalking)
        m_tNextState.eNextState = PLAYER_STATE::WALK_END;
    // 이동은 제일 마지막에.
    m_Desc->pPlayerTransform->Go_Straight(fTimeDelta);

    return m_tNextState;
}

void CPlayer_BattleWalkState::End()
{
}

CPlayer_BattleWalkState* CPlayer_BattleWalkState::Create(void* pArg)
{
    _bool isLanding = { false };
    _bool isEvade = { false };

    if (nullptr != pArg)
    {
        PLAYER_BATTLEWALK_DESC* pDesc = static_cast<PLAYER_BATTLEWALK_DESC*>(pArg);
        isEvade = pDesc->isEvade;
        isLanding = pDesc->isLand;
    }

    return new CPlayer_BattleWalkState(isLanding, isEvade);
}

void CPlayer_BattleWalkState::Free()
{
    __super::Free();
}
