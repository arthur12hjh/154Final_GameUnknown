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

	void						SetLevelMainCamera();
private:
#ifdef _DEBUG
	CGameManager*				m_pGameManager = nullptr;

	CImGuiManager*				m_pImGuiManager = nullptr;
	char						m_szSelectCamera[MAX_PATH] = {};
	char						m_szCameraComboTag[MAX_PATH] = {};

	char						m_szSelectLight[MAX_PATH] = {};
	char						m_szVisbility[MAX_PATH] = {};
	CLight*						m_pSelectLight = nullptr;

	CCamera*					m_pSelectCamera = nullptr;
	CTransform*					m_pCameraTransform = nullptr;

	_float						m_vTeleportPoint[3] = {};
	_float						m_fGameSpeed = {1.f};
	_float						m_fFreeCamSpeed = { 1.f };
	_float						m_fCamDistance = { 10.f };

	_bool						m_bIsCamLerp = false;
	_bool						m_bIsFreeCamera = false;
	_bool						m_bIsCameraLock[2] = { false, false };
	_bool						m_bIsTeleport = false;
	_bool						m_ShowLightInfo = false;
#endif // _DEBUG

private :
	void						DrawObjectDebug();
	void						DrawCameraDebug();
	void						DrawLightDebug();
	void						DrawGameSpeedDebug();

public:
	static	CDebugCheatUI*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END