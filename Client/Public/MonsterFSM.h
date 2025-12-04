#pragma once
#include "Client_Defines.h"
#include "StateMachine.h"

NS_BEGIN(Client)
class CMonsterFSM final : public CStateMachine
{
public :
	enum class MONSTER_STATE { IDLE, ATTACK, HIT, MOVE, MIMESIS, DEAD, END };

private :
	CMonsterFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMonsterFSM() = default;

public:
	//Create 에다가 initalize만들어서 합시다.
	virtual		HRESULT							Initialize(void* pArg = nullptr) override;
	virtual		void							Update(_float fTimeDelta) override;

	virtual		HRESULT							Change_State(const WCHAR* LayerTag, void* pArg = nullptr, _bool bIsForce = false) override;
	virtual		HRESULT							Add_State(const WCHAR* StateTag, CState* pNewState) override;

	const MONSTER_STATE&						GetMonsterState() { return m_eMonsterState; }

private :
	MONSTER_STATE								m_eMonsterState = { MONSTER_STATE::END };

public:
	static	CMonsterFSM*						Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CComponent*							Clone(void* pArg) override;
	virtual	void								Free() override;

};
NS_END