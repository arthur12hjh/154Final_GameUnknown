#pragma once
#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CCamera_BeatSaber final : public CCamera
{
public:
	typedef struct tagCamera_Player : public CCamera::CAMERA_DESC
	{
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
		_float		fMouseSensor;
	}CAMERA_PLAYER_DESC;

private:
	CCamera_BeatSaber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_BeatSaber(const CCamera_BeatSaber& Prototype);
	virtual ~CCamera_BeatSaber() = default;

public:
	virtual		HRESULT			Initialize_Prototype() override;
	virtual		HRESULT			Initialize(void* pArg) override;
	virtual		void			Priority_Update(_float fTimeDelta) override;
	virtual		void			Update(_float fTimeDelta) override;
	virtual		void			Late_Update(_float fTimeDelta) override;
	virtual		HRESULT			Render() override;

private:
	class CGameManager*					m_pGameManager = { nullptr };

public:
	static	CCamera_BeatSaber*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END