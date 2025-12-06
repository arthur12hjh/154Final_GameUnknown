#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_BetaTripletState final : public CPlayerState
{
private:
	CPlayer_BetaTripletState();
	virtual ~CPlayer_BetaTripletState() = default;

public:
	//상태 진입 전에 진입 가능한 녀석인지 체크합니다. 플레이어 정보 요구하거나 하는 녀석들은 다 얘 있어야 편해요
	static _bool CanEnter(class CPlayer* pPlayer, PLAYER_DESC* pPlayerDesc);
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void						 Start(void* pArg = nullptr, _float fBlendRatio = 0.12f) override;
	virtual PLAYER_TRANSITION_DESC       Update(_float fTimeDelta) override;
	virtual _float End() override;

private:
	_bool m_HasLockedOn = { false }; 

public:
	static	CPlayer_BetaTripletState* Create(void* pArg);
	virtual	void			   Free() override;

};

NS_END
