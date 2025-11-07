#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CDebugProfiler : public CBase
{
private:
	CDebugProfiler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDebugProfiler() = default;

public:
	HRESULT					Initialize();

	void					Update(_float fTimeDeleta);
	HRESULT					Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };


public:
	static CDebugProfiler*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;

};
NS_END