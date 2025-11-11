#pragma once

#include "Tool_UI_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool_UI)

class CUI_MainApp final : public CBase
{	
private:
	CUI_MainApp();
	virtual ~CUI_MainApp() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	class CGUIManager*		m_pGuiManager = { nullptr };

private:
	HRESULT Ready_Default_Setting();
	HRESULT Start_Level(LEVEL eLevelID);
	HRESULT Ready_Prototypes();

public:
	static CUI_MainApp* Create();
	virtual void Free() override;
};

NS_END



