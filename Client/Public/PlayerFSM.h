#pragma once

#include "Client_Defines.h"
#include "PlayerState.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CPlayerFSM final : public CBase
{
private:
	CPlayerFSM();
	virtual ~CPlayerFSM() = default;

public:
	//enum °ª ¹ÝÈ¯.
	PLAYER_STATE					Get_StateEnum();
	class CPlayerState*				Get_CurrentState() { return m_pCurrentState;  }
	void							Clear_FSM();
public:
	virtual HRESULT					Initialize(void* pArg = nullptr);
	virtual void					Update(_float fTimeDelta);
	virtual class CPlayerState*		Create_State(PLAYER_TRANSITION_DESC tTransitionDesc);

	void	Handle_Transition(PLAYER_TRANSITION_DESC& Desc);
	void	Evaluate_ModeTransitions(_float fTimeDelta, PLAYER_TRANSITION_DESC& Desc);

private:
	CGameInstance*					m_pGameInstance = { nullptr };
	class CGameManager*				m_pGameManager = { nullptr };
	class CPlayerState*				m_pCurrentState = { nullptr };
	class CPlayer*					m_pPlayer = { nullptr };
	struct Player_Desc*				m_pPlayerDesc = { nullptr };

private:
	virtual HRESULT					Ready_State();
	void							Change_State(class CPlayerState* pPlayerState);
public:
	static CPlayerFSM*		Create();
	virtual	void			Free() override;

};
NS_END