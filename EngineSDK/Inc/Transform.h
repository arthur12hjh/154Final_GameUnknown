#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec;
		_float		fRotationPerSec;

	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& Prototype);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) const {
		return XMLoadFloat4(reinterpret_cast<const _float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]));		
	}

	_float3 Get_Scale() const ;

	const _float4x4* Get_WorldMatrixPtr() const {
		return &m_WorldMatrix;
	}

	void					Set_State(STATE eState, _vector vState);

	void					Set_Scale(_vector vScale);
	void					Set_Rotation(_vector vRotation, _bool bIsQuaternion = false);
	void					Set_Scale(_float fX, _float fY, _float fZ);

	/* 현재는 가장 업데이트가 늦게 도는 Physx Manager Update 단에서 돌고 있음. */
	void					Update_PreWorldMatrix();
public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

public:
	/* 이전 프레임 월드 매트릭스 넣을 수 있게 변경. */
	HRESULT					Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _bool IsPreWorldMatrix = false);

public:
	void					Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void					Go_Backward(_float fTimeDelta);
	void					Go_Left(_float fTimeDelta);
	void					Go_Right(_float fTimeDelta);

	void					Move_Direction(_float fTimeDelta, _vector vDir, _float fSpeed);

	void					Turn(_fvector vAxis, _float fTimeDelta);
	void					Rotation(_fvector vAxis, _float fRadian);
	void					Rotation(_float fRadianX, _float fRadianY, _float fRadianZ);
	void					Rotation(_float fQuatX, _float fQuatY, _float fQuatZ, _float fQuatW);

	void					LookAt(_fvector vAt);

	void					LookAt_Lerp(_fvector vAt, _float fRatio, _float fSpeed = 1.f);
	void					Chase_Lerp(_fvector vTargetPos, _float fTimeDelta, _float fLimitDistance = 0.f);
	void					Change_Look(_fvector vLook); 

private:
	class CGameObject*		m_pOwner = nullptr;

	_float					m_fSpeedPerSec = {};
	_float					m_fRotationPerSec = {};
	_float4x4				m_WorldMatrix{};
	_float4x4				m_PreWorldMatrix{};

public:
	static CTransform*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END