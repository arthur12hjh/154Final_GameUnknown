#pragma once

#include "Tool_UI_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
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
	

private:
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pContext{ nullptr };

private:
	CGameInstance* m_pGameInstance = { nullptr };

private:

public:
	virtual void Free() override;
};

NS_END
