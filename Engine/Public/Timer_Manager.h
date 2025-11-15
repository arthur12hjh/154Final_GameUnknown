#pragma once


#include "Base.h"

NS_BEGIN(Engine)

class CTimer_Manager final : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

	HRESULT				Initialize();

public:
	void				Update_Timer(_float fDeletaTime);

public:
	HRESULT				Add_Timer(const _wstring& strTimerTag);
	void				Compute_TimeDelta(const _wstring& strTimerTag);
	_float				Get_TimeDelta(const _wstring& strTimerTag);

	void				ADD_DelayFunction(const WCHAR*	szTimerName, _float fAfterTime, function<void()> Function);

private:		
	map<const _wstring, class CTimer*>				m_Timers;
	
	unordered_map<_wstring, _float>					m_TimerTime;
	unordered_map<_wstring, function<void()>>		m_TimerFunction;

private:
	class CTimer* Find_Timer(const _wstring& strTimerTag);

public:
	static CTimer_Manager* Create();
	virtual void Free();
};

NS_END