#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_BetaChargingSlashState final : public CPlayerState
{
private:
	CPlayer_BetaChargingSlashState();
	virtual ~CPlayer_BetaChargingSlashState() = default;

public:
	static _bool CanEnter(class CPlayer* pPlayer, PLAYER_DESC* pPlayerDesc);
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void						 Start(void* pArg = nullptr) override;
	virtual PLAYER_TRANSITION_DESC       Update(_float fTimeDelta) override;
	virtual void						 End() override;

private:
	_bool						m_isStartCharge = { false };
	_bool						m_isLoopCharge = { false };
	_bool						m_isAttack = { false };
public:
	static	CPlayer_BetaChargingSlashState* Create(void* pArg);
	virtual	void			   Free() override;

};

NS_END
