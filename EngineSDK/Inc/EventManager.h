#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CEventHandle;

class CEventManager final : public CBase
{
private :
	CEventManager();
	virtual ~CEventManager() = default;

public :
	HRESULT									Add_Event(const WCHAR* szEventTag, CEventHandle* pEvent);
	HRESULT									Remove_Event(const WCHAR* szEventTag);

	HRESULT									Bind_Observer(const WCHAR* szEventTag, CEventHandle* pEvent);
	HRESULT									UnBind_Observer(const WCHAR* szEventTag, CEventHandle* pEvent);

private :
	unordered_map<_wstring, CEventHandle*>	m_Events;

public :
	static	CEventManager*					Create();
	virtual void							Free() override;

};
NS_END