#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_EvadeState final : public CPlayerState
{
private:
	CPlayer_EvadeState();
	virtual ~CPlayer_EvadeState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void			   Start(void* pArg = nullptr) override;
	virtual CPlayerState* Update(_float fTimeDelta) override;
	virtual void			   End() override;

private:
	_bool m_isBackStep = { false };
public:
	static	CPlayer_EvadeState* Create(void* pArg);
	virtual	void			   Free() override;

};

NS_END
