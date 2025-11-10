#include "pch.h"
#include "UIHUD.h"

#include "GameInstance.h"

CUIHUD::CUIHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameHUD{ pDevice, pContext }
{
}

CTexture* CUIHUD::Find_Texture(const _wstring& szTextureTag)
{
	auto	iter = m_Textures.find(szTextureTag);
	if (iter == m_Textures.end())
		return nullptr;

	return iter->second;
}

HRESULT CUIHUD::Add_Texture(_uint iProtoLevel, const _wstring& szTextureProtoTag, const _wstring& szTextureTag, void* pArg)
{
	if (nullptr != Find_Texture(szTextureTag))
		return E_FAIL;

	CTexture* pTexture = dynamic_cast<CTexture*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iProtoLevel, szTextureProtoTag, pArg));
	if (nullptr == pTexture)
		return E_FAIL;

	m_Textures.emplace(szTextureTag, pTexture);

	Safe_AddRef(pTexture);

	return S_OK;
}

CTexture* CUIHUD::Get_TextureCom(const WCHAR* szTextureTag)
{
	auto pTexture = m_Textures.find(szTextureTag);

	if (pTexture == m_Textures.end())
		return nullptr;

	return pTexture->second;
}

CUIHUD* CUIHUD::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIHUD* pInstance = new CUIHUD(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIHUD::Free()
{
	__super::Free();
}
