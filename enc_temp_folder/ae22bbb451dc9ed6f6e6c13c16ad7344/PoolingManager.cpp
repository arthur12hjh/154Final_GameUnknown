#include "pch.h"
#include "PoolingManager.h"

#include "GameInstance.h"
#include "Nayitba.h"

CPoolingManager::CPoolingManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    m_pDevice(pDevice),
    m_pContext(pContext),
    m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPoolingManager::Setting_PoolManager(_uint iLevelID)
{
    if (!m_PoolingList.empty())
        Clear_PoolManager();

    switch (LEVEL(iLevelID))
    {
    case LEVEL::GAMEPLAY :
            return Ready_GamePlayPool();
        break;
    }
}

CGameObject* CPoolingManager::SetActivePoolObject(_uint iLevel, const WCHAR* pLayerName, const WCHAR* szPoolTag)
{
    auto iter = m_PoolingList.find(szPoolTag);
    if (iter == m_PoolingList.end())
        return nullptr;

    auto& PoolObjects = iter->second;
    CGameObject* pPoolObject = { nullptr };
    if (!PoolObjects.empty())
    {
        pPoolObject = PoolObjects.back();
        PoolObjects.pop_back();

        // 여기서 레이어에도 추가해주자
        m_pGameInstance->ADD_ToLayer(iLevel, pLayerName, pPoolObject);
    }

    return pPoolObject;
}

void CPoolingManager::UnActivePoolObject(const WCHAR* szPoolTag, CGameObject* pObject)
{
    // 레이어에 있는 오브젝트를 제거하고
    // 반환된 오브젝트를 특정 Pool에 넣는다.
    auto iter = m_PoolingList.find(szPoolTag);
    if (iter == m_PoolingList.end())
    {
        pObject->Set_Dead(true);
        return;
    }

    auto PoolObjects = iter->second;
    Safe_AddRef(pObject);
    pObject->Set_Dead(true);
    PoolObjects.push_back(pObject);
}



HRESULT CPoolingManager::Clear_PoolManager()
{
    for (auto& pair : m_PoolingList)
    {
        for (auto& iter : pair.second)
            Safe_Release(iter);

        pair.second.clear();
    }
    m_PoolingList.clear();

    return S_OK;
}

HRESULT CPoolingManager::Ready_GamePlayPool()
{
#pragma region Setting Monster Pool
    vector<CGameObject*> PoolList = {};
    PoolList.reserve(1000);

    CNayitba::NAYITBA_DESC NaytibaDesc = {};
    NaytibaDesc.iMonsterID = 7;

    for (_uint i = 0; i < 100; ++i)
    {
        CBase* pBase = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Nayitba"), &NaytibaDesc);
        if (nullptr == pBase)
            return E_FAIL;
        PoolList.push_back(static_cast<CGameObject*>(pBase));
    }

    m_PoolingList.emplace(TEXT("Monster_Pool"), move(PoolList));
#pragma endregion

   
    return S_OK;
}

CPoolingManager* CPoolingManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CPoolingManager(pDevice, pContext);
}

void CPoolingManager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);

    Clear_PoolManager();
}
