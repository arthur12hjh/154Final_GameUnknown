#pragma once

#include "Tool_UI_Defines.h"
#include "Level.h"


/* 2. 자원을 로드하는 동안, 화면에 로딩 상태를 표현해준다. */

NS_BEGIN(Tool_UI)

class CUI_Level_Loading final : public CLevel
{
private:
	CUI_Level_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CUI_Level_Loading() = default;

public:
	virtual HRESULT Initialize(LEVEL eNextLevelID);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVEL				m_eNextLevelID = { LEVEL::END };
	class CUI_Loader*		m_pLoader = { nullptr };
	_bool m_bLevelTransitioning{ false };

private:
	HRESULT Ready_Prototypes();
	HRESULT Ready_Layer_BackGround();

public:
	static CUI_Level_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, LEVEL eNextLevelID);
	virtual void Free() override;
};

NS_END