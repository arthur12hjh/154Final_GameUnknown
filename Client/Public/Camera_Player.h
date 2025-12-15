#pragma once

#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CCamera_Player final : public CCamera
{
public:
	typedef struct tagCamera_Player : public CCamera::CAMERA_DESC
	{
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
		_float		fMouseSensor;
	}CAMERA_PLAYER_DESC;

private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& Prototype);
	virtual ~CCamera_Player() = default;

public:
	virtual		HRESULT			Initialize_Prototype() override;
	virtual		HRESULT			Initialize(void* pArg) override;
	virtual		void			Priority_Update(_float fTimeDelta) override;
	virtual		void			Update(_float fTimeDelta) override;
	virtual		void			Late_Update(_float fTimeDelta) override;
	virtual		HRESULT			Render() override;

	void		Set_Distance(_float fDistance) { m_fDistance = fDistance; }


private:
	class CGameManager*			m_pGameManager = { nullptr };
	class CTransform*			m_pPlayerTransform = { nullptr };

	_float4x4					m_CombinedWorldMatrix = {};

	_float						m_fMouseSensor = { 0.f };
	_float						m_fRotateX = { 0.f };
	_float						m_fRotateY = { 0.f };
	_float						m_fRotateZ = { 0.f };
	_float						m_fYaw = {};
	_float						m_fPitch = {};

	_float						m_fDistance = { 10.f };
	
	_float3						m_vPivot = { 1.5f, 4.f, 0.f };

public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END