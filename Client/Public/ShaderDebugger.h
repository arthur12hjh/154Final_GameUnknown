#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CLight;
NS_END

NS_BEGIN(Client)

class CGameManager;
class CImGuiManager;

class CShaderDebugger final : public CGameObject
{
private:
	CShaderDebugger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CShaderDebugger() = default;

public:
	HRESULT		Initialize();
	void		Update(_float fTimeDeleta);
	HRESULT		Render();

private:
	class CGameInstance* m_pGameInstance = { nullptr };

private:

public:
	static CShaderDebugger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END