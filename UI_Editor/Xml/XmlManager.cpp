#include "pch.h"

#include "XmlManager.h"
#include "GameInstance.h"

CXmlManager::CXmlManager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CXmlManager::Load_Default_Layer(const _char* pFilePath)
{
    return S_OK;
}

void CXmlManager::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
