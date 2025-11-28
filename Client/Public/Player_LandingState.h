#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_LandingState final : public CPlayerState
{
public:
	enum class JUMP_TYPE { RUN, WALK, IDLE };
private:
	CPlayer_LandingState();
	virtual ~CPlayer_LandingState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void			   Start(void* pArg = nullptr) override;
	virtual PLAYER_TRANSITION_DESC	   Update(_float fTimeDelta) override;
	virtual void			   End() override;

public:
	JUMP_TYPE m_eType = {};
	_bool     m_isLanding = false;

public:
	static	CPlayer_LandingState* Create(void* pArg);
	virtual	void				Free() override;

};

NS_END
