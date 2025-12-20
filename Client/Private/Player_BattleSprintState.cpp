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
        m_pPlayer->Set_Animation("proto_Battle_jump_Sprint_Short", false, 1.2f, 0.05f, false);
}

PLAYER_TRANSITION_DESC CPlayer_BattleSprintState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    _vector vCameraLook = XMVector3Normalize(XMVectorSetY(XMLoadFloat4(m_pGameInstance->Get_CamLook()), 0.f));
    _bool isWalking = { false };

    m_fRadian = 0.f;

    _float fDirX{}, fDirY{};

    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) &&
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
        fDirY += 1.f;
    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) &&
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
        fDirY -= 1.f;
    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D) &&
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
        fDirX += 1.f;
    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) &&
        m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
        fDirX -= 1.f;

    // 입력 유무
    _bool isInput = !(0.f == fDirX && 0.f == fDirY);

    if (true == isInput)
    {
        m_iNoInputFrameCount = 0;
        isWalking = true;

        m_fRadian = atan2f(fDirX, fDirY);

        _matrix matRot = XMMatrixRotationY(m_fRadian);
        vCameraLook = XMVector3Normalize(XMVectorSetY(XMVector3TransformNormal(vCameraLook, matRot), 0.f));

        XMStoreFloat4(&m_vLastMoveLook, vCameraLook);
    }
    else
    {
        ++m_iNoInputFrameCount;

        if (m_iNoInputFrameCount <= 8)
        {
            isWalking = true;
            vCameraLook = XMVector3Normalize(XMVectorSetY(XMLoadFloat4(&m_vLastMoveLook), 0.f));
        }
    }

    _vector vPlayerLook = XMVector3Normalize(XMVectorSetY(m_Desc->pPlayerTransform->Get_State(STATE::LOOK), 0.f));

    _float fTurnDotThreshold = -0.90f;
    _float fTurnDotRelease = -0.85f;

    _float fDot = XMVectorGetX(XMVector3Dot(vPlayerLook, vCameraLook));

    // 락 해제
    if (true == m_isTurnLocked && fDot > fTurnDotRelease)
        m_isTurnLocked = false;

    // Turn 애니 종료 시 Run 복귀
    if (true == m_isSprintStart && (18.f/20.f <= fAnimationRatio) && false == m_isLanding)
    {
        m_pPlayer->Set_Animation("Proto_Battle_Sprint", true, 1.2f, 0.f);
        m_isSprintStart = false;
        m_isTurning = false;
        m_isAlreadyTurned = false;
    }
    else if (true == m_isSprintStart && (22.f/30.f <= fAnimationRatio) && true == m_isLanding)
    {
        m_pPlayer->Set_Animation("Proto_Battle_Sprint", true, 1.2f, 0.f);
        m_isSprintStart = false;
        m_isTurning = false;
        m_isAlreadyTurned = false;
    }

    if (true == m_isTurning && (52.f / 54.f) <= fAnimationRatio)
    {
        m_pPlayer->Set_Animation("Proto_Battle_Sprint", true, 1.2f, 0.12f);
        m_isTurning = false;
        m_isAlreadyTurned = false; 
    }

    if (true == m_isTurning && (19.f  / 54.f) <= fAnimationRatio)
        m_isMovable = true;

    if (true == m_isMovable)
    {
        _vector vResult = XMVectorLerp(vPlayerLook, vCameraLook, 0.15f);
        m_Desc->pPlayerTransform->Change_Look(vResult);
    }
    else if(true == m_isTurning && false == m_isAlreadyTurned && false == m_isMovable)
    {
        _vector vResult = XMVectorLerp(vPlayerLook, vCameraLook, 1.f);
        m_Desc->pPlayerTransform->Change_Look(vResult);
        m_isAlreadyTurned = true;
    }

    if (true == m_isEvading && true == isAnimFinished && false == m_isTurning)
    {
        m_pPlayer->Set_Animation("Proto_Battle_Sprint", true, 1.2f, 0.12f);
        m_isSprintStart = false;
    }
    else if (true == m_isSprintStart && true == isAnimFinished && false == m_isTurning)
    {
        m_pPlayer->Set_Animation("Proto_Battle_Sprint", true, 1.2f, 0.12f);
        m_isSprintStart = false;
    }

    // Turn 트리거 (입력 있을 때만 트리거)
    if (false == m_isTurning && false == m_isTurnLocked && false == m_isSprintStart && false == m_isLanding &&
        false == m_isEvading && true == isWalking && true == isInput && fDot <= fTurnDotThreshold)
    {
        // 좌/우 판정: cross.y 부호로 선택
        _vector vCross = XMVector3Cross(vPlayerLook, vCameraLook);
        _float fCrossY = XMVectorGetY(vCross);

        m_pPlayer->Set_Animation("Proto_Battle_Sprint_Turn_L", false, 1.5f, 0.f, false, -1.f, 2.5f, true);
        m_isTurning = true;
        m_isTurnLocked = true;
        m_isMovable = false;
    }

    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
    {
        PLAYER_JUMP_DESC Desc;
        Desc.isSprintJump = true;

        m_tNextState.eNextState = PLAYER_STATE::JUMP;
        m_tNextState.pArg = &Desc;
    }
    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
        m_tNextState.eNextState = PLAYER_STATE::EVADE;
    else if (m_pGameInstance->KeyDown(KEY_INPUT::MOUSE, ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)))
        m_tNextState.eNextState = PLAYER_STATE::LIGHT_ATTACK;
    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_E) || m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E))
        m_tNextState.eNextState = PLAYER_STATE::PARRY;
    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_1))
        m_tNextState.eNextState = PLAYER_STATE::BETA_TRIPLET;
    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_2))
        m_tNextState.eNextState = PLAYER_STATE::BETA_CHARGINGSLASH;

    if (false == isWalking)
        m_tNextState.eNextState = PLAYER_STATE::WALK_END;

    // (1) 크게 회전(턴 애니 재생 중)에는 이동 적용하지 않음
    if (false == m_isTurning || true == m_isMovable)
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
