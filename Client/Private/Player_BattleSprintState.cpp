#include "pch.h"
#include "Player_BattleSprintState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_BattleSprintState::CPlayer_BattleSprintState(_bool isLanding, _bool isEvading )
    : CPlayerState{}
    , m_isEvading{ isEvading }
    , m_isLanding{ isLanding }
{
}

void CPlayer_BattleSprintState::Start(void* pArg, _float fBlendRatio)
{
    m_eState = PLAYER_STATE::SPRINT;
    m_isSprintStart = true;

    if(false == m_isLanding)
        m_pPlayer->Set_Animation("Proto_Battle_Sprint_Start", false, 1.2f, 0.05f);
    else if (true == m_isLanding)
        m_pPlayer->Set_Animation("proto_Battle_jump_Sprint_Short", false, 1.2f, 0.05f);
}

PLAYER_TRANSITION_DESC CPlayer_BattleSprintState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    _vector vCameraLook = XMVector3Normalize(XMVectorSetY(XMLoadFloat4(m_pGameInstance->Get_CamLook()), 0.f));
    _bool isWalking = { false };

    m_fDegree = 0.f;

	if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
    {
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
    }

    _matrix matRot = XMMatrixRotationY(XMConvertToRadians(m_fDegree));
    vCameraLook = XMVector3Normalize(XMVectorSetY(XMVector3TransformNormal(vCameraLook, matRot), 0.f));

    // A나 D를 누르면 카메라 벡터를 기준으로 좌우로 움직이게끔 세팅.
    _vector vPlayerLook = XMVector3Normalize(XMVectorSetY(m_Desc->pPlayerTransform->Get_State(STATE::LOOK), 0.f));
    _vector vResult = XMVectorLerp(vPlayerLook, vCameraLook, 0.15f);
    m_Desc->pPlayerTransform->Change_Look(vResult);


    if (true == m_isSprintStart && 0.9f <= fAnimationRatio) //|| (true == m_isChangingDir && fDot <= 5.f))
    {
        m_pPlayer->Set_Animation("Proto_Battle_Sprint", true, 1.2f, 0.12f);
        m_isSprintStart = false;
    }
    else if ((true == m_isSprintStart && true == m_isLanding) && (22.f/30.f) <= fAnimationRatio)
    {
        m_pPlayer->Set_Animation("Proto_Battle_Sprint", true, 1.2f, 0.12f);
        m_isSprintStart = false;
    }

    //if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
    //    m_tNextState.eNextState = PLAYER_STATE::JUMP;
    //if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
    //    m_tNextState.eNextState = PLAYER_STATE::EVADE;

    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
    {
        PLAYER_JUMP_DESC Desc;
        Desc.isSprintJump = true;

        m_tNextState.eNextState = PLAYER_STATE::JUMP;
        m_tNextState.pArg = &Desc;
    }
     
    else if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
        m_tNextState.eNextState = PLAYER_STATE::LIGHT_ATTACK;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_E) ||
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E))
        m_tNextState.eNextState = PLAYER_STATE::PARRY;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_1))
        m_tNextState.eNextState = PLAYER_STATE::BETA_TRIPLET;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
        m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;

    else if (false == isWalking)
        m_tNextState.eNextState = PLAYER_STATE::WALK_END;
    // 이동은 제일 마지막에.
    m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 2.f);

    return m_tNextState;
}

_float CPlayer_BattleSprintState::End()
{
    return m_fNextBlendRatio;
}

CPlayer_BattleSprintState* CPlayer_BattleSprintState::Create(void* pArg)
{
    _bool isLanding = { false };
    _bool isEvade = { false };

    if (nullptr != pArg)
    {
        PLAYER_BATTLEWALK_DESC* pDesc = static_cast<PLAYER_BATTLEWALK_DESC*>(pArg);
        isEvade = pDesc->isEvade;
        isLanding = pDesc->isLand;
    }

    return new CPlayer_BattleSprintState(isLanding, isEvade);
}

void CPlayer_BattleSprintState::Free()
{
    __super::Free();
}
