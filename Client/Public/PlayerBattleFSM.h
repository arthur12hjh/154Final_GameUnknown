#pragma once

#include "Client_Defines.h"
#include "PlayerFSM.h"

NS_BEGIN(Client)

class CPlayerBattleFSM : public CPlayerFSM
{
private:
	CPlayerBattleFSM();
	virtual ~CPlayerBattleFSM() = default;

public:
	virtual HRESULT			Initialize(void* pArg = nullptr) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Change_FSM(PLAYER_STATE eNextState) override;
	virtual class CPlayerState* Create_State(PLAYER_TRANSITION_DESC tTransitionDesc) override;

private:
	_float m_fBattleToIdle = { 0.f };

private:
	virtual HRESULT			Ready_State() override;
	//Battle to Idle 로직 체크
	_bool	Check_BattleToIdle(_float fTimeDelta);
	
public:
	static	CPlayerBattleFSM*	Create();
	virtual	void				Free() override;

};
NS_END