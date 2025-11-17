#include "pch.h"
#include "UIResourceStore.h"

#include "GameInstance.h"

// ½Ì±ÛÅæ ±¸Çö
IMPLEMENT_SINGLETON(CUIResourceStore);

HRESULT CUIResourceStore::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pGameInstance = CGameInstance::GetInstance();

	if (m_pGameInstance == nullptr)
		return E_FAIL;

	Safe_AddRef(m_pGameInstance);
	return S_OK;
}

void CUIResourceStore::Release_UI_Resource_Store()
{
	DestroyInstance();
}

HRESULT CUIResourceStore::Add_UI_Texture(_uint iProtoLevel, const _wstring& szTextureProtoTag, const _wstring& szTextureTag, const _wstring& szFilePath, _uint iTextureIndex, void* pArg)
{
	if (nullptr != Find_UI_Texture_Desc(szTextureTag).pTexture)
		return E_FAIL;

	CTexture* pTexture = dynamic_cast<CTexture*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iProtoLevel, szTextureProtoTag, pArg));
	if (nullptr == pTexture)
		return E_FAIL;

	UI_TEXTURE_RESOURCE_DESC Desc{};

	Desc.pTexture = pTexture;
	Desc.iTextIndex = iTextureIndex;
	wcscpy_s(Desc.szFilePath, szFilePath.c_str());
	wcscpy_s(Desc.szProtoTag, szTextureProtoTag.c_str());

	m_TextureDescs.emplace(szTextureTag, Desc);

	return S_OK;
}

CUIResourceStore::UI_TEXTURE_RESOURCE_DESC CUIResourceStore::Get_UI_Texture_Desc(const WCHAR* szTextureTag)
{
	auto Desc = m_TextureDescs.find(szTextureTag);

	if (Desc == m_TextureDescs.end())
		return UI_TEXTURE_RESOURCE_DESC();

	return Desc->second;
}

void CUIResourceStore::Clear_UI_Texture_Descs()
{
	for (auto& iter : m_TextureDescs)
		Safe_Release(iter.second.pTexture);
	m_TextureDescs.clear();
}

CUIResourceStore::UI_TEXTURE_RESOURCE_DESC CUIResourceStore::Find_UI_Texture_Desc(const _wstring& szTextureTag)
{
	auto	iter = m_TextureDescs.find(szTextureTag);

	if (iter == m_TextureDescs.end())
		return UI_TEXTURE_RESOURCE_DESC();
	
	return iter->second;
}

void CUIResourceStore::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	for (auto& iter : m_TextureDescs)
		Safe_Release(iter.second.pTexture);
	m_TextureDescs.clear();
}
