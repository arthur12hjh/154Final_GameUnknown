#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"

NS_BEGIN(Client)

class CPlayerFSM final : public CComponent 
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
	CPlayerFSM(const CPlayerFSM& rhs);
	virtual ~CPlayerFSM() = default;

public:
	HRESULT						Initialize_Prototype() override;
	HRESULT						Initialize(void* pArg = nullptr) override;
	void						Update(_float fTimeDelta);


private:
	FSM_STATE					m_eStateTag = { FSM_STATE::M_PROTO_BATTLE_IDLE };
	class CPlayerState*			m_pCurrentState = { nullptr };

private:
	HRESULT						Ready_State();

public:
	static	CPlayerFSM*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual  CComponent*	Clone(void* pArg) override;
	virtual	void			Free() override;

};
NS_END