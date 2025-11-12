#include "pch.h"
#include "UIResourceStore.h"

#include "GameInstance.h"

CUIResourceStore::CUIResourceStore()
{
}

HRESULT CUIResourceStore::Initialize()
{
	m_pGameInstance = CGameInstance::GetInstance();

	if (m_pGameInstance == nullptr)
		return E_FAIL;

	Safe_AddRef(m_pGameInstance);
	return S_OK;
}

HRESULT CUIResourceStore::Add_UI_Texture(_uint iProtoLevel, const _wstring& szTextureProtoTag, const _wstring& szTextureTag, const _wstring& szFilePath, _uint iTextureIndex, void* pArg)
{
	if (nullptr != Find_UI_Texture_Desc(szTextureTag).pTexture)
		return E_FAIL;

	CTexture* pTexture = dynamic_cast<CTexture*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iProtoLevel, szTextureProtoTag, pArg));
	if (nullptr == pTexture)
		return E_FAIL;

	UI_TEXTURE_DESC Desc{};

	Desc.pTexture = pTexture;
	Desc.iTextIndex = iTextureIndex;
	Desc.szFilePath = szFilePath;

	m_TextureDescs.emplace(szTextureTag, Desc);

	return S_OK;
}

CUIResourceStore::UI_TEXTURE_DESC CUIResourceStore::Get_UI_Texture_Desc(const WCHAR* szTextureTag)
{
	auto Desc = m_TextureDescs.find(szTextureTag);

	if (Desc == m_TextureDescs.end())
		return UI_TEXTURE_DESC();

	return Desc->second;
}

CUIResourceStore::UI_TEXTURE_DESC CUIResourceStore::Find_UI_Texture_Desc(const _wstring& szTextureTag)
{
	auto	iter = m_TextureDescs.find(szTextureTag);

	if (iter == m_TextureDescs.end())
		return UI_TEXTURE_DESC();
	
	return iter->second;
}

CUIResourceStore* CUIResourceStore::Create()
{
	CUIResourceStore* pInstance = new CUIResourceStore();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CUIResourceStore");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIResourceStore::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	for (auto& iter : m_TextureDescs)
		Safe_Release(iter.second.pTexture);
}
