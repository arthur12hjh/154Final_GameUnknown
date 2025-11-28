#pragma once

#include "Client_Defines.h"
#include "PlayerFSM.h"

NS_BEGIN(Client)

class CPlayerLockonFSM : public CPlayerFSM
{
private:
	CPlayerLockonFSM();
	virtual ~CPlayerLockonFSM() = default;

public:
	virtual HRESULT			Initialize(void* pArg = nullptr) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Change_FSM(PLAYER_STATE eNextState) override;
	virtual class CPlayerState* Create_State(PLAYER_TRANSITION_DESC tTransitionDesc) override;

private:
	virtual HRESULT			Ready_State() override;

public:
	static	CPlayerLockonFSM* Create();
	virtual	void				Free() override;

};
NS_END