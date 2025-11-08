#include "pch.h"
#include "GameManager.h"

#include "GameInstance.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CGameManager);

HRESULT CGameManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (FAILED(Setting_Manager(pDevice, pContext)))
        return E_FAIL;

    return S_OK;
}

void CGameManager::Bind_GameCharacter(CGameObject* pCharacter)
{
    if (m_pPlayer == pCharacter)
        return;

    if (m_pPlayer)
        Safe_Release(m_pPlayer);

    m_pPlayer = pCharacter;
    Safe_AddRef(m_pPlayer);
}

CGameObject* CGameManager::GetGameCharacter()
{
    Safe_AddRef(m_pPlayer);
    return m_pPlayer;
}

HRESULT CGameManager::Setting_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;
    m_pGameInstance = CGameInstance::GetInstance();

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);

    return S_OK;
}

void CGameManager::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
