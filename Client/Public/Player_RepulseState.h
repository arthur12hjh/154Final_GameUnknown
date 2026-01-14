#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_RepulseState final : public CPlayerState
{
public:
	enum class REPULSE_STATE { ATTACK_START, ATTACK_FLOAT, ATTACK_END, END };

private:
	CPlayer_RepulseState();
	virtual ~CPlayer_RepulseState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void						Start(void* pArg = nullptr, _float fBlendRatio = 0.12f) override;
	virtual PLAYER_TRANSITION_DESC	    Update(_float fTimeDelta) override;
	virtual _float End() override;

private:
	REPULSE_STATE m_eAnimState = { REPULSE_STATE::END };
	_bool		  m_isGravityActivated = { false };
	_bool		  m_isSoundPlayed = { false };
	_float		  m_fLerp = { 0.2f };
public:
	static	CPlayer_RepulseState* Create(void* pArg);
	virtual	void			   Free() override;

};

NS_END
