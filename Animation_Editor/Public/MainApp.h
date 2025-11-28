#pragma once

#include "Animation_Editor_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Animation_Editor)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	class CTool_Manager* m_pTool_Manager = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	ID3D11BlendState* m_pBlendState = { nullptr };
	ID3D11DepthStencilState* m_pDepthStencilState = { nullptr };

	_bool						m_isDebuggingActive;


	_float						m_fTimeRate = 1.f;


private:
	HRESULT Ready_Default_Setting();
	HRESULT Start_Level(LEVEL eLevelID);
	HRESULT Ready_Prototypes();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

NS_END



