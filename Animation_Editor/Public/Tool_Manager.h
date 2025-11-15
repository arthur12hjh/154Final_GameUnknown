#pragma once

#include "Animation_Editor_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Animation_Editor)

class CTool_Manager final : public CBase
{
public:
	DECLARE_SINGLETON(CTool_Manager);
private:
	CTool_Manager();
	virtual ~CTool_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT Render();

public:
	void Set_Active(_bool bIsActive);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CGameInstance* m_pGameInstance = { nullptr };
	class CImGui_Manager* m_pImGuiManager = { nullptr };

public:
	void Release_Manager();
	virtual void Free();
};

NS_END