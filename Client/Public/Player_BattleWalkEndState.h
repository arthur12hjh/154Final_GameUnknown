#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_BattleWalkEndState final : public CPlayerState
{
private:
	CPlayer_BattleWalkEndState();
	virtual ~CPlayer_BattleWalkEndState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void					Start(void* pArg = nullptr);
	virtual PLAYER_TRANSITION_DESC			Update(_float fTimeDelta);
	virtual void					End();

private:

public:
	static	CPlayer_BattleWalkEndState*   Create(void* pArg);
	virtual	void					Free() override;

};

NS_END
