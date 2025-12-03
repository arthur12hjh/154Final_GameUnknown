#pragma once

#include "Tool_UI_Defines.h"
#include "Base.h"

/* 1. 다음 레벨에 대한 자원을 로드한다. */

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CGameManager;
NS_END

NS_BEGIN(Tool_UI)

class CUIResourceStore;

class CUI_Loader final : public CBase
{
private:
	CUI_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Loader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading();
	void Output();

public:
	_bool isFinished() const {
		return m_isFinished;
	}

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	LEVEL				m_eNextLevelID = { LEVEL::END };

	HANDLE				m_hThread = {};

	_wstring			m_strMessage = {};
	_bool				m_isFinished = { false };

	CRITICAL_SECTION	m_CriticalSection = {};

	CGameInstance*		m_pGameInstance = { nullptr };
	CUIResourceStore*	m_pUIResourceStore = { nullptr };

private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_GamePlay();
	HRESULT Loading_UI_For_Logo_Level();
	HRESULT Loading_UI_For_GamePlay_Level();

	HRESULT Loading_UI_For_Combat_HUD_Vitals();
	HRESULT Loading_UI_For_Combat_HUD_Skills();
	HRESULT Loading_UI_For_World();

public:
	static CUI_Loader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;

};

NS_END