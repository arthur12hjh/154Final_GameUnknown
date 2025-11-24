#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"

NS_BEGIN(Client)

class CPlayerFSM final : public CStateMachine
{
public:
	enum class FSM_STATE
	{ 
		M_PROTO_BATTLE_IDLE,
		M_PROTO_BATTLE_WALK,
		M_PROTO_BATTLE_SPRINT,
		M_PROTO_BATTLE_JUMP_FORWARD,
		M_PROTO_SWORD_EVADE,
		M_PROTO_SWORD_LIGHTATTACK,
		M_PROTO_GUARD,
		M_END
	};

private:
	CPlayerFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPlayerFSM() = default;

public:

	virtual HRESULT				Initialize(void* pArg = nullptr) override;
	virtual void				Update(_float fTimeDelta) override;

	HRESULT						Change_State(const WCHAR* LayerTag, void* pArg = nullptr);
	const						FSM_STATE& GetFSMState() { return m_eState; }

private:
	FSM_STATE					m_eState = { FSM_STATE::M_PROTO_BATTLE_IDLE };

private:
	virtual HRESULT				Add_State(const WCHAR* StateTag, CState* pNewState) override;

	HRESULT						Ready_State();

public:
	static	CPlayerFSM* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	//virtual	CStateMachine* Clone(void* pArg) override;
	virtual	void			Free() override;

};
NS_END