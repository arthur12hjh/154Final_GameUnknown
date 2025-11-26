#pragma once

#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Engine)
//class CBoxCollider;
NS_END

NS_BEGIN(Client)

class CCamera_Free final : public CCamera
{
public:
	typedef struct tagCamera_Free : public CCamera::CAMERA_DESC
	{
		_float		fMouseSensor;
	}CAMERA_FREE_DESC;

private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& Prototype);
	virtual ~CCamera_Free() = default;

public:
	virtual		HRESULT			Initialize_Prototype() override;
	virtual		HRESULT			Initialize(void* pArg) override;
	virtual		void			Priority_Update(_float fTimeDelta) override;
	virtual		void			Update(_float fTimeDelta) override;
	virtual		void			Late_Update(_float fTimeDelta) override;
	virtual		HRESULT			Render() override;

	void						SetCameraAnimation(const _float4x4*	StartLerpMatrix, const _float4x4* EndLerpMatrix, _bool bIsLerp = true);
	void						CameraLock(_bool bIsKeyBoard = false, _bool bIsMouse = false);
	void						GetCameraLock(_bool(&pOut)[2]);

private:
	_bool						m_bIsCameraAnimation = { false };
	_bool						m_bIsLock[2] = {false};

	_float						m_fAccTime = {};
	_float						m_fLerpTime = { 5.f };
	_float4x4					m_fStartLerpMatrix = {};
	_float4x4					m_fEndLerpMatrix = {};

	_float						m_fMouseSensor = {};

	// UITool에서 쓰는 중 나중에 지우겠습니다
	//CBoxCollider*					m_pColliderCom{ nullptr };

public:
	static CCamera_Free*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END