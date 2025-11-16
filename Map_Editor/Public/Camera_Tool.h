#pragma once
#include "MapTool_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CCinemaComponent;
NS_END

NS_BEGIN(Tool_Map)
class CCamera_Tool final : public CBase
{
public  :
	static const WCHAR*			m_szCinemaComponentName;

private  :
	CCamera_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCamera_Tool() = default;

public:
	HRESULT						Initialize();

	void						Priority_Update(_float fTimeDelta);
	void						Update(_float fTimeDelta);
	void						Late_Update(_float fTimeDelta);

	HRESULT						Render();

	HRESULT						Save_Camera_Action();
	HRESULT						Load_Camera_Actions();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	CGameInstance*				m_pGameInstance = { nullptr };
	CCinemaComponent*			m_pCameraAnimation = { nullptr };

	_float						m_vRotation[3];
	_float						m_vTranslate[3];

	_float						m_fFov = {};
	_float						m_fFar = {};
	_float						m_fNear = {};



public:
	static	CCamera_Tool*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void				Free() override;
};

NS_END