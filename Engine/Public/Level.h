#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CGameHUD;

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelID);	
	virtual ~CLevel() = default;

public:
	_uint Get_LevelID() const {
		return m_iID;
	}

public:
	virtual HRESULT				Initialize();
	virtual void				Update(_float fTimeDelta);
	virtual HRESULT				Render();

	// HUD 세팅하는 함수
	// HUD 세팅안하면 UI 안나옴
	void						SetHUD(CGameHUD* pGameHUD);

	// 안에서 레퍼런스 카운트 증가합니다 내려주세요
	// 제발 내려주세요
	CGameHUD*					GetHUD();

protected:
	_uint						m_iID = {};
	class CGameInstance*		m_pGameInstance = { nullptr };

	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	CGameHUD*					m_pHUD = { nullptr };

public:
	virtual void				Free() override;
};

NS_END