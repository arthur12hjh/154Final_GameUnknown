#include "pch.h"
#include "Player_ParryState.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_ParryState::CPlayer_ParryState(_bool isImmediate)
	: CPlayerState{}
	, m_isImmediate{ isImmediate }
{
}

void CPlayer_ParryState::Start(void* pArg, _float fBlendRatio)
{
	m_eState = PLAYER_STATE::PARRY;
	m_Desc->isLookFixed = true;

	if (true == m_isImmediate)
	{
		m_isParryStart = false;
		m_pPlayer->Set_Animation("Proto_Guard_idle", false, 1.2f, fBlendRatio);
		m_Desc->isParryable = true;
		m_Desc->isJustParryable = true;
	}
	else
	{
		m_isParryStart = true;
		m_pPlayer->Set_Animation("Proto_Guard_Fast", false, 1.4f);
	}
}

PLAYER_TRANSITION_DESC CPlayer_ParryState::Update(_float fTimeDelta)
{
	_bool isAnimationFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	// 가드 중 타임델타 쌓기
	if (false == m_isParryStart)
		m_fGuardTimeAcc += fTimeDelta;

	// 패링 시작 모션이 끝났다면, 가드 idle 애니메이션으로 전환
	if (true == m_isParryStart && true == isAnimationFinished)
	{
		m_pPlayer->Set_Animation("Proto_Guard_idle", true, 1.2f, false);
		m_isParryStart = false;
	}

	// 패링 스타트하고 패링 올리는 애니메이션이 얼마 남지  않은 상태부턴,
	// 패링 판정이 가능해진다.
	if (true == m_isParryStart && fAnimationRatio >= 0.4f)
	{
		m_Desc->isParryable = true;
		m_Desc->isJustParryable = true;
	}
	// 저스트 패링 판정은 0.2초 이상 가드하고 있으면 사라짐.
	if (m_fGuardTimeAcc >= 0.2f)
		m_Desc->isJustParryable = false;

	// 가드 키에서 손 뗴거나 가드가 3초이상 지속 됐다면
	if ((false == m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_E))
		|| m_fGuardTimeAcc >= 3.f)
		m_tNextState.eNextState = PLAYER_STATE::PARRY_END;

	return m_tNextState;
}

_float CPlayer_ParryState::End()
{
	m_Desc->isParryable = false;
	m_Desc->isJustParryable = false;
	m_Desc->isLookFixed = false;

	return m_fNextBlendRatio;
}

CPlayer_ParryState* CPlayer_ParryState::Create(void* pArg)
{
	PLAYER_PARRY_DESC* pDesc;
	_bool isImmediate = { false };

	if (nullptr != pArg)
	{
		pDesc = static_cast<PLAYER_PARRY_DESC*>(pArg);
		isImmediate = pDesc->isImmediate;
	}
	
	return new CPlayer_ParryState(isImmediate);
}

void CPlayer_ParryState::Free()
{
	__super::Free();
}
