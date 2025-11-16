#pragma once

#include "Tool_UI_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CTexture;
class CGameInstance;
NS_END

NS_BEGIN(Tool_UI)
class CUIResourceStore final : public CBase
{
	DECLARE_SINGLETON(CUIResourceStore);

public:
	typedef struct tagUITextureDesc
	{
		CTexture* pTexture{ nullptr };
		_uint iTextIndex{ 0 };
		WCHAR szFilePath[MAX_PATH]{};
		WCHAR szProtoTag[MAX_PATH]{};
		/*_wstring szFilePath{};
		_wstring szProtoTag{};*/
	}UI_TEXTURE_RESOURCE_DESC;

private:
	CUIResourceStore() = default;
	virtual ~CUIResourceStore() = default;

public:	
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Release_UI_Resource_Store();

public:
	HRESULT Add_UI_Texture(_uint iProtoLevel, const _wstring& szTextureProtoTag, const _wstring& szTextureTag, const _wstring& szFilePath, _uint iTextureIndex = 0, void* pArg = nullptr);
	UI_TEXTURE_RESOURCE_DESC Get_UI_Texture_Desc(const WCHAR* szTextureTag);
	unordered_map<_wstring, UI_TEXTURE_RESOURCE_DESC>* Get_UI_Texture_Descs(){ return &m_TextureDescs; }
	void Clear_UI_Texture_Descs();

private:
	UI_TEXTURE_RESOURCE_DESC Find_UI_Texture_Desc(const _wstring& szTextureTag);

private:
	CGameInstance* m_pGameInstance{ nullptr };
	unordered_map<_wstring, UI_TEXTURE_RESOURCE_DESC> m_TextureDescs{};

public:
	virtual void Free() override;
};

NS_END