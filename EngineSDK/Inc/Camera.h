#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float3		vEye{}, vAt{};
		_float		fFov{}, fNear{}, fFar{};
	}CAMERA_DESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


	// 카메라의 정보를 수정한다.
	// 숫자를 더해서 넘겨주면 각 플래그에 맞게 수정됨
	// ex) 10 : Near 와 Aspect 수정
	//	0001(1) : FOV Only Edit			0010(2) : Near Only Edit				
	//	0100(4) : Far Only Edit			1000(8) : Aspect Only Edit
	void						SetCameraInfo(const CAMERA_INFO& CameraInfo, bitset<4> bitFlag = 15);

	const CAMERA_INFO&			GetCameraInfo() { return m_pCameraInfo; }

protected:
	CAMERA_INFO					m_pCameraInfo = {};

protected:
	HRESULT Bind_Matrices();


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

NS_END