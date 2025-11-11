#pragma once

#include "Tool_UI_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CHUDLayer;
NS_END

NS_BEGIN(Client)
class CUIHUD;
NS_END

NS_BEGIN(Tool_UI)
class CGUIManager : public CBase
{
	DECLARE_SINGLETON(CGUIManager);

private:
	CGUIManager();
	virtual ~CGUIManager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Update(_float fTimeDelta);
	void Render();
	void Release_GUI_Manager();

private:
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pContext{ nullptr };

private:
	CGameInstance* m_pGameInstance{ nullptr };

	_uint m_iCurrentLevel = 0;
	_uint m_iPrevLevel = 0;

	class Client::CUIHUD* m_pUIHUD{ nullptr };

	vector<_wstring> m_ViewModes{};
	_wstring m_szCurViewMode{ TEXT("Editor") };

	unordered_map<_wstring, CHUDLayer*>	m_pLayers;

	vector<_wstring> m_ProtoTags = {};
	vector<_wstring> m_LayerTags = {};
	_wstring m_strCurrentProtoTag{};
	_wstring m_strCurrentLayerTag{};

private:
	void ViewMode();

	void Editor_Window();
	void Show_UIObject_List();

	void Clone_UI();

public:
	virtual void Free() override;
};

NS_END
