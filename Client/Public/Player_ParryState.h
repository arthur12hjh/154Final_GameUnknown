#pragma once

#include "PlayerState.h"


/*
패링은 상태 자세히 나누는게 맞다고 판단해서 일단 분리.
*/

NS_BEGIN(Client)

class CPlayer_ParryState final : public CPlayerState
{
private:
	CPlayer_ParryState(_bool isImmediate);
	virtual ~CPlayer_ParryState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void						Start(void* pArg = nullptr, _float fBlendRatio = 0.12f) override;
	virtual PLAYER_TRANSITION_DESC	    Update(_float fTimeDelta) override;
	virtual _float End() override;

public:
	_float m_fGuardTimeAcc = { 0.f };
	_bool  m_isParryStart = { false };
	_bool  m_isImmediate = { false };
public:
	static	CPlayer_ParryState* Create(void* pArg);
	virtual	void			   Free() override;

};

NS_END
