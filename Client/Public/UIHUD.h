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
	HRESULT Load_Data(_wstring szLayerTag);

	void Set_Show_Debug_Rect(_bool bShow) {
		m_bShowDebugRect = bShow;
	}

	_bool Get_Show_Debug_Rect() { return m_bShowDebugRect;}

private:
	void Save_Hierarchy(CUIBase* pUI, Json& OutData, _bool bIsRoot);
	void Load_Hierarchy(CUIBase* pUIParent, Json jData);

private:
	string WStringToUTF8(const _wstring& wstr);
	_wstring UTF8ToWString(const string& str);

	_bool m_bShowDebugRect{ true };

public:
	static CUIHUD* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END