#pragma once

#include "Client_Defines.h"
#include "GameHUD.h"
#include "UIBase.h"

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

	HRESULT Save_Data(_wstring szLayerTag);
	HRESULT Load_Data(const _tchar* szFilePath);

private:
	void Save_Hierarchy(CUIBase* pUI, Json& OutData);

public:
	static CUIHUD* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END