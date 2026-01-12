#include "EventManager.h"

#include "EventHandle.h"

CEventManager::CEventManager()
{
}

HRESULT CEventManager::Add_Event(const WCHAR* szEventTag, CEventHandle* pEvent)
{
	auto iter = m_Events.find(szEventTag);
	if (iter == m_Events.end())
		m_Events.emplace(szEventTag, pEvent);
	else
	{
		Safe_Release(iter->second);
		iter->second = pEvent;
	}

	Safe_AddRef(pEvent);
	return S_OK;
}

HRESULT CEventManager::Remove_Event(const WCHAR* szEventTag)
{
	auto iter = m_Events.find(szEventTag);
	if (iter == m_Events.end())
		return E_FAIL;

	Safe_Release(iter->second);
	m_Events.erase(iter);
	return S_OK;
}

HRESULT CEventManager::Bind_Observer(const WCHAR* szEventTag, CEventHandle* pEvent)
{
	auto iter = m_Events.find(szEventTag);
	if (iter == m_Events.end())
		return E_FAIL;

	iter->second->Bind_Event(pEvent);
	return S_OK;
}

HRESULT CEventManager::UnBind_Observer(const WCHAR* szEventTag, CEventHandle* pEvent)
{
	auto iter = m_Events.find(szEventTag);
	if (iter == m_Events.end())
		return E_FAIL;

	iter->second->UnBind_Event(pEvent);
	return S_OK;
}

CEventManager* CEventManager::Create()
{
	return new CEventManager();
}

void CEventManager::Free()
{
	for (auto& iter : m_Events)
		Safe_Release(iter.second);

	m_Events.clear();
}