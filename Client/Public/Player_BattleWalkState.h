#pragma once

#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_BattleWalkState final : public CPlayerState
{
public:
	enum class RUN_DIR { STRAIGHT, LEFT, RIGHT, BACKWARD };
private:
	CPlayer_BattleWalkState(_bool isLanding = false, _bool isEvading = false);
	virtual ~CPlayer_BattleWalkState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void					Start(void* pArg = nullptr, _float fBlendRatio = 0.12f);
	virtual PLAYER_TRANSITION_DESC			Update(_float fTimeDelta);
	virtual _float End();

private:
	_bool		m_isRunStart = { false };
	_bool		m_isLanding = { false }; 
	_bool		m_isEvading = {  false };
	_float		m_fRadian = { 0.f }; 

	//
	_bool		m_isTurning = { false };
	_bool		m_isTurnLocked = { false };
	_float4		m_vLastMoveLook = { 0.f, 0.f, 1.f, 0.f }; 
	_float		m_fNoInputTimer = { 0.f };
	_uint		m_iNoInputFrameCount = { 0 };
	_bool		m_isAlreadyTurned = { false };
	_bool		m_isMovable = { true };
public:
	static	CPlayer_BattleWalkState*		Create(void* pArg);
	virtual	void					Free() override;

};

NS_END
