#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_LightAttackState final : public CPlayerState
{
public:
	enum class COMBO { LIGHT_ATTACK1, LIGHT_ATTACK2, LIGHT_ATTACK3, LIGHT_ATTACK4, LIGHT_ATTACK5, END };
private:
	CPlayer_LightAttackState();
	virtual ~CPlayer_LightAttackState() = default;

public:
	virtual _bool isTransferAble(PLAYER_MODE ePlayerMode, PLAYER_STATE ePlayerState) override;
public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void			   Start(void* pArg = nullptr) override;
	virtual PLAYER_TRANSITION_DESC	   Update(_float fTimeDelta) override;
	virtual void			   End() override;

	
private:
	COMBO m_eCombo = {};
	_float m_fLimitProgress = { 0.f };
public:
	static	CPlayer_LightAttackState* Create(void* pArg);
	virtual	void			   Free() override;

};

NS_END
