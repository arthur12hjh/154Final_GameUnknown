#pragma once
#include "Client_Defines.h"
#include "StateMachine.h"

NS_BEGIN(Client)
class CMonsterFSM final : public CStateMachine
{
public :
	enum class FSM_STATE { M_IDLE, M_PATROL, M_ATTACK, M_HIT, M_DEAD, M_END };

private :
	CMonsterFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMonsterFSM() = default;

public:
	//Create 에다가 initalize만들어서 합시다.
	virtual		HRESULT							Initialize(void* pArg = nullptr) override;
	virtual		void							Update(_float fTimeDelta) override;

	virtual		HRESULT							Change_State(const WCHAR* LayerTag, void* pArg = nullptr) override;
	const	FSM_STATE&							GetFSMState() { return m_eState; }

private :
	FSM_STATE									m_eState = { FSM_STATE::M_IDLE };

private :
	virtual		HRESULT							Add_State(const WCHAR* StateTag, CState* pNewState) override;

	HRESULT										Ready_State();

public:
	static	CMonsterFSM*						Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CComponent*							Clone(void* pArg) override;
	virtual	void								Free() override;

};
NS_END