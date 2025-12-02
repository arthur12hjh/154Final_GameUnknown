#include "pch.h"

#include "Player_SheatheHairPin.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_SheatheHairpin::CPlayer_SheatheHairpin()
	: CPlayerState {}
{
}

void CPlayer_SheatheHairpin::Start(void* pArg)
{
    m_eState = PLAYER_STATE::SHEATHE_HAIRPIN;
	m_pPlayer->Set_Animation("Proto_Run_Blade_Start", false, 1.2f); 
}

PLAYER_TRANSITION_DESC CPlayer_SheatheHairpin::Update(_float fTimeDelta)
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
    _vector vResult = XMVectorLerp(vPlayerLook, vCameraLook, 0.15f);
    m_Desc->pPlayerTransform->Change_Look(vResult);

    m_Desc->pPlayerTransform->Go_Straight(fTimeDelta);


	if (true == isAnimFinished)
	{
		m_tNextState.eNextState = PLAYER_STATE::WALK_END;
		m_tNextState.eMode = PLAYER_MODE::IDLE;
		m_tNextState.isChangeMode = true;
	}
		
	return m_tNextState;
}

void CPlayer_SheatheHairpin::End()
{
}

CPlayer_SheatheHairpin* CPlayer_SheatheHairpin::Create(void* pArg)
{
	return new CPlayer_SheatheHairpin();
}

void CPlayer_SheatheHairpin::Free()
{
	__super::Free();
}
