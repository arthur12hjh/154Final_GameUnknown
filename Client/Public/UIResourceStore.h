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
	//CTexture* Get_UI_TextureCom(const WCHAR* szTextureTag);
	//_uint Get_UI_Texture_Index(const WCHAR* szTextureTag);

	//const unordered_map<_wstring, CTexture*>* Get_UI_Textures() { return &m_Textures; }
	//const unordered_map<_wstring, _uint>* Get_UI_TextureIndices() { return &m_TextureIndices; }

	HRESULT Add_UI_Texture(_uint iProtoLevel, const _wstring& szTextureProtoTag, const _wstring& szTextureTag, const _wstring& szFilePath, _uint iTextureIndex = 0, void* pArg = nullptr);
	UI_TEXTURE_DESC Get_UI_Texture_Desc(const WCHAR* szTextureTag);
	const unordered_map<_wstring, UI_TEXTURE_DESC>* Get_UI_Texture_Descs(){ return &m_TextureDescs; }

private:
	//CTexture* Find_Texture(const _wstring& szTextureTag);
	UI_TEXTURE_DESC Find_UI_Texture_Desc(const _wstring& szTextureTag);

private:
	CGameInstance* m_pGameInstance{ nullptr };
	//unordered_map<_wstring, CTexture*> m_Textures{};
	//unordered_map<_wstring, _uint> m_TextureIndices{};
	unordered_map<_wstring, UI_TEXTURE_DESC> m_TextureDescs{};

public:
	static CUIResourceStore* Create();
	virtual void Free() override;
};

NS_END