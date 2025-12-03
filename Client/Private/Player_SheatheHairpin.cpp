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
	m_pPlayer->Set_Animation("Eve_Weapon_Stop_End", false, 1.2f, 0.f); 
}

PLAYER_TRANSITION_DESC CPlayer_SheatheHairpin::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

    _vector vCameraLook = XMVector3Normalize(XMVectorSetY(XMLoadFloat4(m_pGameInstance->Get_CamLook()), 0.f));
    _bool isWalking = { false };


	//추후 비녀 가리는 로직도 추가할 것.
	if (9 / 45.f <= fAnimationRatio)
		m_Desc->isWeaponVisible = false;

	if (true == isAnimFinished)
	{
		m_tNextState.eNextState = PLAYER_STATE::IDLE;
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
