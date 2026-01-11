#pragma once

#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)
class CNpc;

class CCamera_Npc final : public CCamera
{
public:
	typedef struct tagCamera_Npc : public CCamera::CAMERA_DESC
	{
		_float		fMouseSensor;
	}Camera_Npc_DESC;

private:
	CCamera_Npc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Npc(const CCamera_Npc& Prototype);
	virtual ~CCamera_Npc() = default;

public:
	virtual		HRESULT			Initialize_Prototype() override;
	virtual		HRESULT			Initialize(void* pArg) override;
	virtual		void			Priority_Update(_float fTimeDelta) override;
	virtual		void			Update(_float fTimeDelta) override;
	virtual		void			Late_Update(_float fTimeDelta) override;
	virtual		HRESULT			Render() override;

	void						SetCameraAnimation(const _float4x4* StartLerpMatrix, const _float4x4* EndLerpMatrix, const _float4 fTargetLookPos, const _float fTargetRadius, _bool bIsReturn = false);
	void						SetPrevLook(const _vector vPrevLook) { m_vPrevLook = vPrevLook; }
	void						ReverseCameraAnimation();
	void						CameraLock(_bool bIsKeyBoard = false, _bool bIsMouse = false);

	void						GetCameraLock(_bool(&pOut)[2]);
	void						SetCameraSpeed(_float fCameraSpeed);

	void						SetTargetNpc(CNpc* pNpc);

	_bool						Get_LerpEnd() const { return m_isLerpEnd; }
	_bool						Get_Reverse() const { return m_isReverse; }

private:
	_bool						m_bIsCameraAnimation = { false };
	_bool						m_bIsLock[2] = {false};

	_float						m_fAccTime = {};
	_float						m_fLerpTime = { 1.5f };
	_float						m_fCameraSpeed = { 1.f };

	_float4x4					m_fStartLerpMatrix = {};
	_float4x4					m_fEndLerpMatrix = {};

	_float						m_fMouseSensor = {};

	CNpc*						m_pTargetNpc{ nullptr };
	_bool						m_isLerpEnd{ false };
	_bool						m_isReverse{ false };
	_float						m_fTargetRadius{ 0.f };
	_float4						m_fTargetLookPos{};

	_vector						m_vPrevLook{};

public:
	static CCamera_Npc*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END