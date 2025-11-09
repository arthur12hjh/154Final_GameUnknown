#pragma once

#include "Tool_UI_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
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

public:
	void Set_Current_HUD(class CUIHUD* pHUD)
	{
		m_pUIHUD = pHUD;
	}

private:
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pContext{ nullptr };

private:
	CGameInstance* m_pGameInstance = { nullptr };

	CUIHUD* m_pUIHUD{ nullptr };

	vector<_wstring> m_ViewModes{};
	_wstring m_szCurViewMode{ TEXT("Edit") };

private:
	void ViewMode();

public:
	virtual void Free() override;
};

NS_END
