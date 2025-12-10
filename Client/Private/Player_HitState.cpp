#include "pch.h"
#include "Player_HitState.h"

#include "Player.h"
#include "GameInstance.h"

// 히트 방향따라 다르게 처리해줘야 하긴 하는데..
// 일단 애니메이션 정상화 되면 처리 ㄱㄱ.
CPlayer_HitState::CPlayer_HitState(PLAYER_HIT_DESC* pHitDesc)
	: CPlayerState {}
{
	if (nullptr != pHitDesc)
	{
		m_vImpactDir = pHitDesc->vImpactDir;
		m_vHitPoint = pHitDesc->vHitPoint;
		m_fImpact = pHitDesc->fImpact;
		m_vHitDir = pHitDesc->vHitDir;
		m_vAttackerPos = pHitDesc->vAttackerPos;
	}
}

void CPlayer_HitState::Start(void* pArg, _float fBlendRatio)
{
	m_eState = PLAYER_STATE::HIT;

	m_Desc->isLookFixed = true;
	m_pPlayer->Set_Animation("Result_Hit_Stand_Light_Fw_Lw", false, 1.2f);

	XMStoreFloat3(&m_vDirection, XMVector3Normalize(XMVectorSetY(m_Desc->pPlayerTransform->Get_State(STATE::POSITION) - XMLoadFloat4(&m_vAttackerPos), 0.f)));

}

PLAYER_TRANSITION_DESC CPlayer_HitState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	if(0.45f >= fAnimationRatio)
		m_Desc->pPlayerTransform->Move_Direction(fTimeDelta, XMLoadFloat3(&m_vDirection), 30.f * (0.45f - fAnimationRatio) );

	// 끝나면 Idle 
	if (true == isAnimFinished)
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	// 움직이려고 하면 캔슬해서 걷기
	if ((m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)) 
		&& fAnimationRatio >= 0.3f)
		m_tNextState.eNextState = PLAYER_STATE::WALK;

	else if ((m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E))
		&& fAnimationRatio >= 0.15f)
		m_tNextState.eNextState = PLAYER_STATE::PARRY;

	return m_tNextState;
}

_float CPlayer_HitState::End()
{
	m_Desc->isLookFixed = false;

	return m_fNextBlendRatio;
}

CPlayer_HitState* CPlayer_HitState::Create(void* pArg)
{
	PLAYER_HIT_DESC* pDesc = { nullptr }; 

	if (nullptr != pArg)
	{
		pDesc = static_cast<PLAYER_HIT_DESC*>(pArg);
	}

	return new CPlayer_HitState(pDesc);
}

void CPlayer_HitState::Free()
{
	__super::Free();
}
