#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameObject;
class CGameInstance;

class ENGINE_DLL CState abstract : public CBase
{
public :
	typedef struct StateDesc
	{
		CGameObject*					pOwner;
	}STATE_DESC;

protected :
	CState();
	virtual ~CState() = default;

public :
	//생성 할때 Create 함수에 집어넣어 주세요
	virtual		HRESULT							Initialize(void* pArg);

	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual		void							Start(void* pArg = nullptr, CState* pPreState = nullptr);

	virtual		void							Update(_float fTimeDelta);
	virtual		void							End();

	_bool										Is_FinishedState() { return m_bIsFinished; }
	_bool										Is_EnableChange() { return m_bIsEnableChange; }

	const char*									GetAnimationName();

protected :
	CGameObject*								m_pOwner = { nullptr };
	CGameInstance*				               	m_pGameInstance = { nullptr };

	string										m_szAnimationName = {};
	_uint										m_iSectionIndex = {};
	
	_bool										m_bIsEnableChange = { true };
	_bool										m_bIsFinished = { false };

public :
	virtual	void								Free() override;

};
NS_END