#pragma once

#include "Client_Defines.h"
#include "PlayerState.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

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
	//외부에서 상태 주입할 수 있는 메서드. 좋은 구조는 아닌데 일단 편해서..
	void							Change_State(class CPlayerState* pPlayerState);
public:
	virtual void					Change_FSM(PLAYER_STATE eNextState) = 0;
	virtual HRESULT					Initialize(void* pArg = nullptr) = 0;
	virtual void					Update(_float fTimeDelta) = 0;
	//팩토리 클래스. 각 FSM이 상속받아서 구현해야 한다. 
	virtual class CPlayerState*		Create_State(PLAYER_TRANSITION_DESC tTransitionDesc) = 0;

protected:
	CGameInstance*					m_pGameInstance = { nullptr };
	class CGameManager*				m_pGameManager = { nullptr };
	class CPlayerState*				m_pCurrentState = { nullptr };
	class CPlayer*					m_pPlayer = { nullptr };

protected:
	virtual HRESULT					Ready_State() = 0;

public:
	virtual	void			Free() override;

};
NS_END