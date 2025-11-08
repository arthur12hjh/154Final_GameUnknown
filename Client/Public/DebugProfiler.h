#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CDebugProfiler : public CGameObject
{
private:
	CDebugProfiler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDebugProfiler() = default;

public:
	HRESULT						Initialize();

	void						Update(_float fTimeDeleta);
	HRESULT						Render();

private :
	char						m_szViewPercentText[MAX_PATH];

public:
	static	CDebugProfiler*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};
NS_END