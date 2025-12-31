#pragma once
#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CGameManager;

class CLevel_Ending final : public CLevel
{
private:
	CLevel_Ending(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_Ending() = default;

public:
	virtual HRESULT				Initialize() override;
	virtual void				Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	/* 이 레벨에서 쓰기위한 객체들을 생성한다. */
	HRESULT						Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT						Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT						Ready_Layer_UI(const _wstring& strLayerTag);

private:
	CGameManager*				m_pGameManager{ nullptr };

public:
	static CLevel_Ending*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void				Free() override;
};

NS_END