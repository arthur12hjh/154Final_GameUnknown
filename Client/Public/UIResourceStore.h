#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CTexture;
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CUIResourceStore final : public CBase
{
public:
	typedef struct tagUITextureDesc
	{
		CTexture* pTexture{ nullptr };
		_uint iTextIndex{ 0 };
		_wstring szFilePath;
	}UI_TEXTURE_DESC;

private:
	CUIResourceStore();
	virtual ~CUIResourceStore() = default;

public:	
	HRESULT Initialize();

public:
	HRESULT Add_UI_Texture(_uint iProtoLevel, const _wstring& szTextureProtoTag, const _wstring& szTextureTag, const _wstring& szFilePath, _uint iTextureIndex = 0, void* pArg = nullptr);
	UI_TEXTURE_DESC Get_UI_Texture_Desc(const WCHAR* szTextureTag);
	const unordered_map<_wstring, UI_TEXTURE_DESC>* Get_UI_Texture_Descs(){ return &m_TextureDescs; }

private:
	UI_TEXTURE_DESC Find_UI_Texture_Desc(const _wstring& szTextureTag);

private:
	CGameInstance* m_pGameInstance{ nullptr };
	unordered_map<_wstring, UI_TEXTURE_DESC> m_TextureDescs{};

public:
	static CUIResourceStore* Create();
	virtual void Free() override;
};

NS_END