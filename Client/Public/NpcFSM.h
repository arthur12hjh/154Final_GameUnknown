#pragma once
#include "Client_Defines.h"
#include "StateMachine.h"

NS_BEGIN(Client)
class CNpcFSM final : public CStateMachine
{
public:
	enum class NPC_STATE { IDLE, TALK, PATROL, END };

private:
	CNpcFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CNpcFSM() = default;

public:
	//Create 에다가 initalize만들어서 합시다.
	virtual		HRESULT							Initialize(void* pArg = nullptr) override;
	virtual		void							Update(_float fTimeDelta) override;

	virtual		HRESULT							Change_State(const WCHAR* LayerTag, void* pArg = nullptr, _bool bIsForce = false) override;
	virtual		HRESULT							Add_State(const WCHAR* StateTag, CState* pNewState) override;

	const NPC_STATE&							GetMonsterState() { return m_eMonsterState; }

private:
	NPC_STATE									m_eMonsterState = { NPC_STATE::END };

public:
	static	CNpcFSM*							Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CComponent*							Clone(void* pArg) override;
	virtual	void								Free() override;

};
NS_END