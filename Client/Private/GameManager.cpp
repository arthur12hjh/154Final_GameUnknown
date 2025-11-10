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

HRESULT CGameManager::Ready_UIResourceStore()
{
    m_pUIResourceStore = CUIResourceStore::Create();

    if (!m_pUIResourceStore)
        return E_FAIL;

    Safe_AddRef(m_pUIResourceStore);

    return S_OK;
}

CGameObject* CGameManager::GetGameCharacter()
{
    Safe_AddRef(m_pPlayer);
    return m_pPlayer;
}

#pragma region UIResourceManager
//HRESULT CGameManager::Add_UI_Texture(_uint iProtoLevel, const _wstring& szTextureProtoTag, const _wstring& szTextureTag, _uint iTextureIndex, void* pArg)
//{
//    return m_pUIResourceStore->Add_UI_Texture(iProtoLevel, szTextureProtoTag, szTextureTag, iTextureIndex, pArg);
//}
//
//CTexture* CGameManager::Get_UI_TextureCom(const WCHAR* szTextureTag)
//{
//    return m_pUIResourceStore->Get_UI_TextureCom(szTextureTag);
//}
//
//_uint CGameManager::Get_UI_Texture_Index(const WCHAR* szTextureTag)
//{
//    return m_pUIResourceStore->Get_UI_Texture_Index(szTextureTag);
//}
//
//const unordered_map<_wstring, CTexture*>* CGameManager::Get_UI_Textures()
//{
//    return m_pUIResourceStore->Get_UI_Textures();
//}
//
//const unordered_map<_wstring, _uint>* CGameManager::Get_UI_TextureIndices()
//{
//    return m_pUIResourceStore->Get_UI_TextureIndices();
//}

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
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
