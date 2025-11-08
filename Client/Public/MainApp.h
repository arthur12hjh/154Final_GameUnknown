#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

#ifdef _DEBUG
class CImGuiMain;
#endif

class CMainApp final : public CBase
{	
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT					Initialize();
	void					Update(_float fTimeDelta);
	HRESULT					Render();

private:
#ifdef _DEBUG
	CImGuiMain* m_pImGuiDebug = nullptr;
#endif

	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	HRESULT					Ready_Default_Setting();
	HRESULT					Ready_Manager_Setting();
	HRESULT					Start_Level(LEVEL eLevelID);
	HRESULT					Ready_Gara();
	HRESULT					Ready_Prototypes();

public:
	static CMainApp*		Create();
	virtual void			Free() override;
};

NS_END



