#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CImGuiMain : public CBase
{
private:
	CImGuiMain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImGuiMain() = default;

public:
	HRESULT					Initialize();
	
	void					Update(_float fTimeDeleta);
	HRESULT					Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private :
	ImGuiWindowFlags		m_ImGuiWindowFlags = {};

	_char					m_szFPS[MAX_PATH] = {};
	_char					m_szFramePreivew[MAX_PATH] = { "60" };

	_uint					m_iDrawCnt = {};
	_float					m_fTimeAcc = {};

	_bool					m_bIsGamePause = false;

private:
	HRESULT					Ready_Default_Setting();

public:
	static CImGuiMain*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;
};
NS_END