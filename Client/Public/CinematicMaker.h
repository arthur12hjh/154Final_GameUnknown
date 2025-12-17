#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CLight;
class CCamera;
NS_END

NS_BEGIN(Client)
class CGameManager;
class CImGuiManager;

class CCinematicMaker : public CGameObject
{
private:
	CCinematicMaker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCinematicMaker() = default;

public:
	HRESULT						Initialize();

	void						Update(_float fTimeDeleta);
	HRESULT						Render();

private:
#ifdef _DEBUG
	CGameManager* m_pGameManager = nullptr;
	CImGuiManager* m_pImGuiManager = nullptr;

	map<_uint, CINEMATIC_DESC>* m_pCinematicDatas = { nullptr };
#endif // _DEBUG

private:
	void						WriteCinematicDesc();

	void						Play_Cinematic(_uint iSelectedCinematicIndex);

public:
	static	CCinematicMaker* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	virtual CGameObject* Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END