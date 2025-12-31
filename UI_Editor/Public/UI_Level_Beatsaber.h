#pragma once

#include "Tool_UI_Defines.h"
#include "Level.h"

NS_BEGIN(Tool_UI)

class CUI_Level_Beatsaber final : public CLevel
{
private:
	CUI_Level_Beatsaber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CUI_Level_Beatsaber() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_UI(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);

private:
	_bool m_isOverlay{ true };

public:
	static CUI_Level_Beatsaber* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END