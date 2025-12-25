#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_JumpState final : public CPlayerState
{
private:
	CPlayer_JumpState(_bool isSprintJump = false);
	virtual ~CPlayer_JumpState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void			   Start(void* pArg = nullptr, _float fBlendRatio = 0.12f) override;
	virtual PLAYER_TRANSITION_DESC       Update(_float fTimeDelta) override;
	virtual _float End() override;

private:
	_float	m_fDegree = { 0.f };
	_float  m_fScaleFactor = { 1.f };
	_bool   m_isLanding = { false };
	_bool   m_isSprintJump = { false };
	PLAYER_BATTLEWALK_DESC m_NextStateDesc;

public:
	static	CPlayer_JumpState* Create(void* pArg);
	virtual	void			   Free() override;

};

NS_END
