#include "pch.h"
#include "Player_LockonJustEvadeState.h"

#include "GameInstance.h"
#include "Player.h"

CPlayer_LockonJustEvadeState::CPlayer_LockonJustEvadeState(PLAYER_DIRECTION ePlayerDir)
	: CPlayerState{}
	, m_eDirection { ePlayerDir }
{
}

void CPlayer_LockonJustEvadeState::Start(void* pArg, _float fBlendRatio)
{
	m_eState = PLAYER_STATE::JUST_EVADE;

	if (PLAYER_DIRECTION::STRAIGHT == m_eDirection)
		m_pPlayer->Set_Animation("Proto_Lockon_Sword_Just_Evade1_Forward_3", false, 1.2f);
	else if (PLAYER_DIRECTION::LEFT == m_eDirection)
		m_pPlayer->Set_Animation("Proto_Lockon_Sword_Just_Evade1_Left_3", false, 1.2f);
	else if (PLAYER_DIRECTION::RIGHT == m_eDirection)
		m_pPlayer->Set_Animation("Proto_Lockon_Sword_Just_Evade1_Right_3", false, 1.2f);
	else if (PLAYER_DIRECTION::BACKWARD == m_eDirection)
		m_pPlayer->Set_Animation("Proto_Lockon_Sword_Just_Evade1_backward_3", false, 1.2f);
}

PLAYER_TRANSITION_DESC CPlayer_LockonJustEvadeState::Update(_float fTimeDelta)
{
	_bool isAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	//ratio 받아와서 세팅
	_float fAnimationRatio = m_pPlayer->Get_AnimationRatio();

	if (true == isAnimFinished)
		m_tNextState.eNextState = PLAYER_STATE::IDLE;

	//키입력을 이용한 캔슬도 필요함.

    return m_tNextState;
}

_float CPlayer_LockonJustEvadeState::End()
{
    return m_fNextBlendRatio;
}

CPlayer_LockonJustEvadeState* CPlayer_LockonJustEvadeState::Create(void* pArg)
{
	PLAYER_DIRECTION eDirection = { PLAYER_DIRECTION::END };

	if (nullptr != pArg)
	{
		PLAYER_DIRECTION_DESC* pDesc = static_cast<PLAYER_DIRECTION_DESC*>(pArg);
		eDirection = pDesc->eDir;
	}

    return new CPlayer_LockonJustEvadeState(eDirection);
}

void CPlayer_LockonJustEvadeState::Free()
{
	__super::Free();
}
