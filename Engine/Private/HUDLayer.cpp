#include "HUDLayer.h"

#include "GameObject.h"

HRESULT CHUDLayer::Initialize_Layer()
{
    return S_OK;
}

void CHUDLayer::Priority_Update(_float fDeletaTime)
{
    for (auto& iter : m_UserInterfaces)
    {
        iter.second->Priority_Update(fDeletaTime);
    }
}

void CHUDLayer::Update(_float fDeletaTime)
{
    for (auto& iter : m_UserInterfaces)
    {
        iter.second->Update(fDeletaTime);
    }
}

void CHUDLayer::Late_Update(_float fDeletaTime)
{
    for (auto& iter : m_UserInterfaces)
    {
        iter.second->Late_Update(fDeletaTime);
    }
}

HRESULT CHUDLayer::ADD_GameObject(const WCHAR* UITag, CGameObject* pUserInterface)
{
    auto iter = m_UserInterfaces.find(UITag);
    if (iter != m_UserInterfaces.end())
        return E_FAIL;

    m_UserInterfaces.emplace(UITag, pUserInterface);
    return S_OK;
}

HRESULT CHUDLayer::Remove_GameObject(const WCHAR* UITag)
{
    auto iter = m_UserInterfaces.find(UITag);
    if (iter != m_UserInterfaces.end())
        return E_FAIL;

    Safe_Release(iter->second);
    m_UserInterfaces.erase(iter);
    return S_OK;
}

CGameObject* CHUDLayer::Find_GameObject(const WCHAR* UITag)
{
    auto iter = m_UserInterfaces.find(UITag);
    if (iter == m_UserInterfaces.end())
        return nullptr;

    return iter->second;
}

CHUDLayer* CHUDLayer::Create()
{
    CHUDLayer* pLayer = new CHUDLayer();
    if (FAILED(pLayer->Initialize_Layer()))
    {
        Safe_Release(pLayer);
        MSG_BOX("Create Fail : HUD Layer");
    }
    return pLayer;
}

void CHUDLayer::Free()
{
    __super::Free();

    for (auto& iter : m_UserInterfaces)
        Safe_Release(iter.second);

    m_UserInterfaces.clear();
}
