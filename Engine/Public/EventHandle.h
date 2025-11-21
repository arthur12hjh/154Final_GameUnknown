#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class ENGINE_DLL CEventHandle abstract : public CBase
{
protected:
	CEventHandle(function<void(void*)> Func);
	virtual ~CEventHandle() = default;

public :
	// 이벤트 등록
	void			Bind_Event(CEventHandle* pEvent);

	// 이벤트 제거
	void			UnBind_Event(CEventHandle* pEvent);

	// 이벤트 호출
	void			Notify(void* pData);
	_bool			GetEventCall() { return m_bIsEventCall; }

protected:
	list<CEventHandle*>			m_pOvservers = {};
	function<void(void*)>		m_pEventFunc = { nullptr };

	_bool						m_bIsEventCall = false;

public :
	virtual		void			Free() override;
};
NS_END