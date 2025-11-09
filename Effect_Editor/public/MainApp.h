#pragma once

#include "Tool_Effect_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool_Effect)

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
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	HRESULT Ready_Default_Setting();
	HRESULT Start_Level(LEVEL eLevelID);
	HRESULT Ready_Prototypes();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

NS_END



