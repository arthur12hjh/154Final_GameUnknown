#pragma once

#include "Client_Defines.h"
#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayerFSM abstract : public CBase
{
protected:
	CPlayerFSM();
	virtual ~CPlayerFSM() = default;

public:
	//enum 값 반환.
	PLAYER_STATE					Get_StateEnum();
	class CPlayerState*				Get_CurrentState() { return m_pCurrentState;  }
	void							Clear_FSM();

public:
	virtual void					Change_FSM(PLAYER_STATE eNextState) = 0;
	virtual HRESULT					Initialize(void* pArg = nullptr) = 0;
	virtual void					Update(_float fTimeDelta) = 0;
	//팩토리 클래스. 각 FSM이 상속받아서 구현해야 한다. 
	virtual class CPlayerState*		Create_State(PLAYER_TRANSITION_DESC tTransitionDesc) = 0;

protected:
	class CPlayerState*				m_pCurrentState = { nullptr };

protected:
	virtual HRESULT					Ready_State() = 0;

public:
	virtual	void			Free() override;

};
NS_END