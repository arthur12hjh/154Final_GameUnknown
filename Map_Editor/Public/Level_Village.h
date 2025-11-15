#pragma once

#include "MapTool_Defines.h"
#include "Level.h"

NS_BEGIN(Tool_Map)

class CLevel_Village final : public CLevel
{
private:
	CLevel_Village(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_Village() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Terrain(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Terrain_Sand(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);

	HRESULT Ready_Village_Mou(const _wstring& strLayerTag);

private:
	class CImgui_Manager* m_pImguiManager = { nullptr };

public:
	static CLevel_Village* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END