#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CState;
class CGameObject;

class ENGINE_DLL CStateMachine abstract : public CBase
{
public:
	typedef struct StateMachineDesc
	{
		CGameObject*					pOwner;
	}STATE_MACHINE_DESC;

protected:
	CStateMachine();
	virtual ~CStateMachine() = default;

public:
	//Create 에다가 initalize만들어서 합시다.
	virtual		HRESULT							Initialize(void* pArg = nullptr);
	virtual		void							Update(_float fTimeDelta);

	virtual		HRESULT							Change_State(const WCHAR* LayerTag) = 0;

protected:
	CGameObject*								m_pOwner = { nullptr };
	
	CState*										m_pCurrentState = nullptr;
	unordered_map<_wstring, CState*>			m_pStates;
	
protected:
	virtual		HRESULT							Add_State(const WCHAR* StateTag, CState* pNewState) = 0;
	CState*										Find_State(const WCHAR* StateTag);

public:
	virtual	void								Free() override;

};
NS_END