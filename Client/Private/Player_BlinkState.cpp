#include "pch.h"

#include "Player_BlinkState.h"

#include "Player.h"
#include "GameInstance.h"
#include "GameManager.h"

CPlayer_BlinkState::CPlayer_BlinkState()
	: CPlayerState{}
{
}

//시간 느려지면서 이펙트 나와야하는 모션. ( + 림라이트도 )
void CPlayer_BlinkState::Start(void* pArg, _float fBlendRatio)
{
	m_pGameInstance->Active_RadialBlur(0.5f, 16, 0.3f);
	//락 못바꾸게 처리
	m_Desc->isLockChangable = false;
	m_Desc->isUsingBlink = true;
	m_Desc->isInvincible = true;

	m_eState = PLAYER_STATE::BLINK_START;
	m_pPlayer->Set_Animation("P_Eve_Sword_Normal_FlashBehindAttack_S", false, 1.2f);
	
	m_pGameInstance->Active_RadialBlur(2.f, 16, 0.5f);
	m_pGameInstance->Active_DoF(true, 0.3f);
	m_pGameInstance->SetGameSpeed(0.2f);
}

PLAYER_TRANSITION_DESC CPlayer_BlinkState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	//ratio 받아와서 세팅
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	// to Monster
	//_float fDofDist =  XMVectorGetX(XMVector3Length(
	//		XMLoadFloat4(m_pGameInstance->Get_CamPosition()) - m_pGameManager->Get_TargetTransform()->Get_State(STATE::POSITION)));
	//m_pGameInstance->Set_DoFInfo(fDofDist);

	// to Player
	_float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) -
		m_Desc->pPlayerTransform->Get_State(STATE::POSITION)));
	m_pGameInstance->Set_DoFInfo(fDist, 2.f, 1.f);

	if (true == isAnimFinished)
		m_tNextState.eNextState = PLAYER_STATE::BLINK_ATTACK;

	return m_tNextState;
}

_float CPlayer_BlinkState::End()
{
	//m_pGameInstance->Active_RadialBlur(0.2f, 16, 0.6f);
	m_Desc->isLockChangable = false;

	return m_fNextBlendRatio;
}

CPlayer_BlinkState* CPlayer_BlinkState::Create(void* pArg)
{
	return new CPlayer_BlinkState();
}

void CPlayer_BlinkState::Free()
{
	__super::Free();
}
