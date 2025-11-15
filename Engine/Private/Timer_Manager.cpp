#include "Timer_Manager.h"

#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{

}

HRESULT CTimer_Manager::Initialize()
{

	return S_OK;
}

_float CTimer_Manager::Get_TimeDelta(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

void CTimer_Manager::ADD_DelayFunction(const WCHAR* szTimerName, _float fAfterTime, function<void()> Function)
{
	auto iter = m_TimerTime.find(szTimerName);
	if (iter != m_TimerTime.end())
		return;

	m_TimerTime.emplace(szTimerName, fAfterTime);
	m_TimerFunction.emplace(szTimerName, Function);
}

void CTimer_Manager::Compute_TimeDelta(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
		return;

	pTimer->Update_Timer();
}

void CTimer_Manager::Update_Timer(_float fDeletaTime)
{
	for (auto TimerTimeiter = m_TimerTime.begin(); TimerTimeiter != m_TimerTime.end();)
	{
		_bool bIsTimerAction = false;

		TimerTimeiter->second -= fDeletaTime;
		if (0.f >= TimerTimeiter->second)
		{
			auto pTimeriter = m_TimerFunction.find(TimerTimeiter->first);
			if (pTimeriter == m_TimerFunction.end())
				continue;

			pTimeriter->second();
			bIsTimerAction = true;
			m_TimerFunction.erase(pTimeriter);
		}

		if (bIsTimerAction)
			TimerTimeiter = m_TimerTime.erase(TimerTimeiter);
		else
			TimerTimeiter++;
	}
}

HRESULT CTimer_Manager::Add_Timer(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	/*m_Timers.insert({ strTimerTag, pTimer });*/

	m_Timers.emplace(strTimerTag, pTimer);

	return S_OK;
}

CTimer* CTimer_Manager::Find_Timer(const _wstring& strTimerTag)
{
	/*auto		iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CTag_Finder(pTimerTag));*/
	auto		iter = m_Timers.find(strTimerTag);

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

CTimer_Manager* CTimer_Manager::Create()
{
	CTimer_Manager* pTimerManager = new CTimer_Manager();
	if (FAILED(pTimerManager->Initialize()))
	{
		Safe_Release(pTimerManager);
		MSG_BOX("Create Fail : Timer Manager");
	}
	return pTimerManager;
}

void CTimer_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Timers)	
		Safe_Release(Pair.second);

	m_Timers.clear();
	
}
