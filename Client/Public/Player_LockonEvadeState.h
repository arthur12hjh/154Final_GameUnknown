#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_LockonEvadeState final : public CPlayerState
{
public:
	enum class EVADE_DIR { STRAIGHT, RIGHT, BACKWARD, LEFT, END };

private:
	CPlayer_LockonEvadeState();
	virtual ~CPlayer_LockonEvadeState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void			   Start(void* pArg = nullptr) override;
	virtual PLAYER_TRANSITION_DESC	   Update(_float fTimeDelta) override;
	virtual void			   End() override;

private:
	EVADE_DIR m_eDirection = {};
	PLAYER_BATTLEWALK_DESC m_tNextStateDesc = {};
public:
	static	CPlayer_LockonEvadeState* Create(void* pArg);
	virtual	void			   Free() override;
};

NS_END
