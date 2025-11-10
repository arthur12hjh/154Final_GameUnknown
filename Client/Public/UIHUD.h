#pragma once

#include "Client_Defines.h"
#include "GameHUD.h"

NS_BEGIN(Engine)
class CHUDLayer;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUIHUD final : public CGameHUD
{
private:
	CUIHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUIHUD() = default;

public:
	unordered_map<_wstring, CHUDLayer*> Get_Layers() { return m_pLayers; }
	
	/*HRESULT Add_Texture(_uint iProtoLevel, const _wstring& szTextureProtoTag, const _wstring& szTextureTag, void* pArg = nullptr);
	CTexture* Get_TextureCom(const WCHAR* szTextureTag);

	unordered_map<_wstring, CTexture*> Get_Textures() { return m_Textures; }

private:
	CTexture* Find_Texture(const _wstring& szTextureTag);

private:
	unordered_map<_wstring, CTexture*> m_Textures{};*/

public:
	static CUIHUD* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END