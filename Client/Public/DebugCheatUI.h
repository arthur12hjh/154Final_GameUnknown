#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CLight;
NS_END

NS_BEGIN(Client)
class CGameManager;
class CImGuiManager;

class CDebugCheatUI : public CGameObject
{
public :
	static	const char*			szGameSpeedCombo[5];
private:
	CDebugCheatUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDebugCheatUI() = default;

public:
	HRESULT						Initialize();

	void						Update(_float fTimeDeleta);
	HRESULT						Render();

private:
#ifdef _DEBUG
	CGameManager*				m_pGameManager = nullptr;

	CImGuiManager*				m_pImGuiManager = nullptr;
	char						m_szSelectCamera[MAX_PATH] = {};
	char						m_szCameraComboTag[MAX_PATH] = {};

	char						m_szSelectLight[MAX_PATH] = {};
	char						m_szVisbility[MAX_PATH] = {};
	CLight*						m_pSelectLight = nullptr;

	_float						m_fGameSpeed = {1.f};
	_bool						m_bIsCamLerp = false;
	_bool						m_bIsTeleport = false;
#endif // _DEBUG

private :
	void						DrawObjectDebug();
	void						DrawCaemraDebug();
	void						DrawLightDebug();
	void						DrawGameSpeedDebug();

public:
	static	CDebugCheatUI*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END