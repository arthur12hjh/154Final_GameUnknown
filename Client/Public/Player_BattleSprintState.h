#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_BattleSprintState final : public CPlayerState
{
private:
	CPlayer_BattleSprintState();
	virtual ~CPlayer_BattleSprintState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void						Start(void* pArg = nullptr, _float fBlendRatio = 0.12f) override;
	virtual PLAYER_TRANSITION_DESC	    Update(_float fTimeDelta) override;
	virtual _float End() override;

private:
	_bool m_isSprintStart = { false };
	_float m_fDegree = { 0.f };

public:
	static	CPlayer_BattleSprintState* Create(void* pArg);
	virtual	void			   Free() override;

};

NS_END
