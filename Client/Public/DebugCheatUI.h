#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Client)
class CGameManager;

class CDebugCheatUI : public CGameObject
{
private:
	CDebugCheatUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDebugCheatUI() = default;

public:
	HRESULT						Initialize();

	void						Update(_float fTimeDeleta);
	HRESULT						Render();

private:
	CGameManager*				m_pGameManager = nullptr;




	_bool						m_bIsTeleport = false;

public:
	static	CDebugCheatUI*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END