#include "pch.h"
#include "Player_HitState.h"

#include "Player.h"
#include "GameInstance.h"

// 히트 방향따라 다르게 처리해줘야 하긴 하는데..
// 일단 애니메이션 정상화 되면 처리 ㄱㄱ.
CPlayer_HitState::CPlayer_HitState(_float3 vImpactDir, _float fImpact)
	: CPlayerState {}
	, m_vImpactDir { vImpactDir }
	, m_fImpact { fImpact }
{
}

void CPlayer_HitState::Start(void* pArg)
{
	m_eState = PLAYER_STATE::HIT;

	m_pPlayer->Set_Animation("Result_Hit_Stand_Light_Fw_Lw", false, 1.2f);
}

PLAYER_TRANSITION_DESC CPlayer_HitState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta, m_Desc->pPlayerTransform, 0.8f);
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	// 끝나면 Idle 
	if (true == isAnimFinished)
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	// 움직이려고 하면 캔슬해서 걷기
	else if ((m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_W) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_S) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_A) ||
		m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_D)) 
		&& fAnimationRatio >= 0.3f)
		m_tNextState.eNextState = PLAYER_STATE::WALK;

	return m_tNextState;
}

void CPlayer_HitState::End()
{
}

CPlayer_HitState* CPlayer_HitState::Create(void* pArg)
{
	_float3 vImpactDir = { 0.f, 0.f, 0.f };
	_float  fImpact = { 0.f };

	if (nullptr != pArg)
	{
		PLAYER_HIT_DESC* pDesc = static_cast<PLAYER_HIT_DESC*>(pArg);
	}

	return new CPlayer_HitState(vImpactDir, fImpact);
}

void CPlayer_HitState::Free()
{
	__super::Free();
}
