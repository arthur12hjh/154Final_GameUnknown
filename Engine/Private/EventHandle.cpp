#include "EventHandle.h"

CEventHandle::CEventHandle(function<void(void*)> Func)
{
	m_pEventFunc = Func;
}

void CEventHandle::Bind_Event(CEventHandle* pEvent)
{
	auto iter = find(m_pOvservers.begin(), m_pOvservers.end(), pEvent);
	if (iter == m_pOvservers.end())
	{
		Safe_AddRef(pEvent);
		m_pOvservers.push_back(pEvent);
	}
}

void CEventHandle::UnBind_Event(CEventHandle* pEvent)
{
	auto iter = find(m_pOvservers.begin(), m_pOvservers.end(), pEvent);
	if (iter != m_pOvservers.end())
	{
		Safe_Release(*iter);
		m_pOvservers.erase(iter);
	}
}

void CEventHandle::Notify(void* pData)
{
	if (m_bIsEventCall)
		return;

	m_bIsEventCall = true;
	if (m_pEventFunc)
		m_pEventFunc(pData);

	for (auto& iter : m_pOvservers)
		iter->Notify(pData);

	m_bIsEventCall = false;
}

void CEventHandle::Free()
{
	__super::Free();

	for (auto& iter : m_pOvservers)
		Safe_Release(iter);

	m_pOvservers.clear();
}
