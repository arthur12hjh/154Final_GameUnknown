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
class CEffectSRV;
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
	class CGameManager*		m_pGameManager = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CEffectSRV*				m_pEffectSRV = { nullptr };
	_bool					m_bIsMouseLock = { false };

private:
	HRESULT					Ready_Default_Setting();
	HRESULT					Ready_Manager_Setting();
	HRESULT					Start_Level(LEVEL eLevelID);
	HRESULT					Ready_Prototypes();
	HRESULT					Ready_Mouse();
	//클라단에서 사용할 후처리 준비
	HRESULT					Ready_ClientDeferred();
	void					MouseLock();

public:
	static CMainApp*		Create();
	virtual void			Free() override;
};

NS_END



