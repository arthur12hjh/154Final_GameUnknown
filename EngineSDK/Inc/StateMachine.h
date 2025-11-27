#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CState;
class CGameObject;

class ENGINE_DLL CStateMachine abstract : public CComponent
{
public :
	typedef struct StateMachineDesc
	{
		CGameObject*				pOwner;
	}STATEMACHINE_DESC;
protected:
	CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CStateMachine() = default;

public:
	//Create 에다가 initalize만들어서 합시다.
	virtual		HRESULT							Initialize(void* pArg = nullptr);
	virtual		void							Update(_float fTimeDelta);

	virtual		HRESULT							Change_State(const WCHAR* LayerTag, void* pArg = nullptr, _bool bIsForce = false) = 0;
	virtual		HRESULT							Add_State(const WCHAR* StateTag, CState* pNewState) = 0;

	CState*										GetCurrentState() { return m_pCurrentState; }

protected:
	CState*										m_pCurrentState = nullptr;
	unordered_map<_wstring, CState*>			m_pStates;
	
protected:
	CState*										Find_State(const WCHAR* StateTag);

public:
	virtual	CComponent*							Clone(void* pArg);
	virtual	void								Free() override;

};
NS_END