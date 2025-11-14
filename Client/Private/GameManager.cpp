#include "pch.h"
#include "GameManager.h"
#include "GameInstance.h"

#include "DataManager.h"

#include "GameObject.h"

IMPLEMENT_SINGLETON(CGameManager);

HRESULT CGameManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (FAILED(Setting_Manager(pDevice, pContext)))
        return E_FAIL;

    m_pDataManager = CDataManager::Create();
    if (nullptr == m_pDataManager)
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

HRESULT CGameManager::Ready_UIResourceStore()
{
    m_pUIResourceStore = CUIResourceStore::Create();

    if (!m_pUIResourceStore)
        return E_FAIL;

    return S_OK;
}

CGameObject* CGameManager::GetGameCharacter()
{
    Safe_AddRef(m_pPlayer);
    return m_pPlayer;
}

#pragma region UIResourceManager
HRESULT CGameManager::Add_UI_Texture(_uint iProtoLevel, const _wstring& szTextureProtoTag, const _wstring& szTextureTag, const _wstring& szFilePath, _uint iTextureIndex, void* pArg)
{
    return m_pUIResourceStore->Add_UI_Texture(iProtoLevel, szTextureProtoTag, szTextureTag, szFilePath, iTextureIndex, pArg);
}
CUIResourceStore::UI_TEXTURE_DESC CGameManager::Get_UI_Texture_Desc(const WCHAR* szTextureTag)
{
    return m_pUIResourceStore->Get_UI_Texture_Desc(szTextureTag);
}
const unordered_map<_wstring, CUIResourceStore::UI_TEXTURE_DESC>* CGameManager::Get_UI_Texture_Descs()
{
    return m_pUIResourceStore->Get_UI_Texture_Descs();
}
void CGameManager::Clear_UI_Texture_Descs()
{
    m_pUIResourceStore->Clear_UI_Texture_Descs();
}


#pragma endregion

#pragma region DataManager
const CHARACTER_SKILL_DESC* CGameManager::Find_SkillData(_uint iSkillID)
{
    return m_pDataManager->Find_SkillData(iSkillID);
}

const BOSS_NETWORK_DESC* CGameManager::Find_BossData(_uint iBossID)
{
    return m_pDataManager->Find_BossData(iBossID);
}
#pragma endregion


HRESULT CGameManager::Setting_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;
    m_pGameInstance = CGameInstance::GetInstance();

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);

    if (FAILED(Ready_UIResourceStore()))
        return E_FAIL;

    return S_OK;
}

void CGameManager::Free()
{
    __super::Free();

    Safe_Release(m_pUIResourceStore);
    Safe_Release(m_pPlayer);
    Safe_Release(m_pDataManager);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
