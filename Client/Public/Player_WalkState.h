#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_WalkState final : public CPlayerState
{
public:
	enum class RUN_DIR { STRAIGHT, LEFT, RIGHT, BACKWARD };
private:
	CPlayer_WalkState(_bool isLanding = false);
	virtual ~CPlayer_WalkState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void					Start(void* pArg = nullptr);
	virtual PLAYER_TRANSITION_DESC			Update(_float fTimeDelta);
	virtual void					End();

private:
	_bool	m_isRunStart = { false };
	_bool   m_isLanding = { false }; 
	_bool   m_isChangingDir = { false }; 
	_float	m_fDegree = { 0.f }; 

public:
	static	CPlayer_WalkState*		Create(void* pArg);
	virtual	void					Free() override;

};

NS_END
