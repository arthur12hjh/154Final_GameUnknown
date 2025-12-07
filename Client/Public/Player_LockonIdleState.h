#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_LockonIdleState final : public CPlayerState
{
private:
	CPlayer_LockonIdleState();
	virtual ~CPlayer_LockonIdleState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void			   Start(void* pArg = nullptr, _float fBlendRatio = 0.12f) override;
	virtual PLAYER_TRANSITION_DESC       Update(_float fTimeDelta) override;
	virtual _float End() override;

public:
	static	CPlayer_LockonIdleState* Create(void* pArg);
	virtual	void			   Free() override;
};

NS_END
