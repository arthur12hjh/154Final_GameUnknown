#include "pch.h"
#include "Player_BattleWalkState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_BattleWalkState::CPlayer_BattleWalkState(_bool isLanding, _bool isEvading)
    : CPlayerState{}
    , m_isEvading{ isEvading }
    , m_isLanding { isLanding }
{
}

void CPlayer_BattleWalkState::Start(void* pArg, _float fBlendRatio)
{
    m_eState = PLAYER_STATE::WALK;

    m_isRunStart = true;

    //Jump Run Start로 시작.
    if (true == m_isEvading && true == m_isLanding)
    {
        m_pPlayer->Set_Animation("Proto_Battle_Run", true, 1.2f, 0.f);
        m_isRunStart = false;
    }
    else if (true == m_isEvading)
        m_pPlayer->Set_Animation("Proto_Battle_Run_StartAfterEvade", false, 1.2f, 0.2f);
    else if (true == m_isLanding)
        m_pPlayer->Set_Animation("Proto_Battle_Jump_Run", false, 1.2f, 0.2f);
    else
        m_pPlayer->Set_Animation("Proto_Battle_Run_Start", false, 1.2f, 0.05f);
}

PLAYER_TRANSITION_DESC CPlayer_BattleWalkState::Update(_float fTimeDelta)
{
    _bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    _vector vCameraLook = XMVector3Normalize(XMVectorSetY(XMLoadFloat4(m_pGameInstance->Get_CamLook()), 0.f));
    _bool isWalking = { false };

    m_fRadian = 0.f;

    _float fDirX{}, fDirY{};

    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W))
        fDirY += 1.f;
    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S))
        fDirY -= 1.f;
    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D))
        fDirX += 1.f;
    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A))
        fDirX -= 1.f;

    //walking false 처리
    if (!(0.f == fDirX && 0.f == fDirY))
        isWalking = true;

    m_fRadian = atan2f(fDirX, fDirY);

    // 입력 유무
    _bool isInput = !(0.f == fDirX && 0.f == fDirY);

    //################################### Input
    if (true == isInput)
    {
        m_fNoInputTimer = 0.f;
        isWalking = true;

        m_fRadian = atan2f(fDirX, fDirY);

        _matrix matRot = XMMatrixRotationY(m_fRadian);
        _vector vPlayerLook = XMVector3Normalize(XMVectorSetY(m_Desc->pPlayerTransform->Get_State(STATE::LOOK), 0.f));
        vCameraLook = XMVector3Normalize(XMVectorSetY(XMVector3TransformNormal(vCameraLook, matRot), 0.f));

        _vector vResult = XMVectorLerp(vPlayerLook, vCameraLook, 0.15f);
        m_Desc->pPlayerTransform->Change_Look(vResult);

        XMStoreFloat4(&m_vLastMoveLook, vCameraLook);
    }
    else
    {
        m_fNoInputTimer += fTimeDelta;
        //0.08초간 입력 안돼도 그냥 달려
        if (m_fNoInputTimer <= 0.08f)
        {
            isWalking = true;
            vCameraLook = XMVector3Normalize(XMVectorSetY(XMLoadFloat4(&m_vLastMoveLook), 0.f));
        }
    }

    //################################### BATTLE RUN
    if(true == m_isLanding  &&  true == isAnimFinished)
    {
        m_pPlayer->Set_Animation("Proto_Battle_Run", true, 1.2f, 0.f, FALSE, 0.f, 8.f, TRUE);
        m_isRunStart = false;
        m_isLanding = false;
        m_isEvading = false;
    }
    if (true == m_isEvading && true == isAnimFinished)
    {
        m_pPlayer->Set_Animation("Proto_Battle_Run", true, 1.2f, 0.12f);
        m_isRunStart = false;
    }
    else if (true == m_isRunStart && true == isAnimFinished) //|| (true == m_isChangingDir && fDot <= 5.f))
    {
        m_pPlayer->Set_Animation("Proto_Battle_Run", true, 1.2f, 0.12f);
        m_isRunStart = false;
    }
    // ################################## Additional Input
    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_SPACE))
        m_tNextState.eNextState = PLAYER_STATE::JUMP;

    else if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_LSHIFT))
        m_tNextState.eNextState = PLAYER_STATE::EVADE;

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
    m_Desc->pPlayerTransform->Go_Straight(fTimeDelta * 1.2f);

    return m_tNextState;
}

_float CPlayer_BattleWalkState::End()
{
    return m_fNextBlendRatio;
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
