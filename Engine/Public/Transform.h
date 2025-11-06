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

	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]), vState);
	}

	void Set_Scale(_float fX, _float fY, _float fZ);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT					Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

public:
	void					Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void					Go_Backward(_float fTimeDelta);
	void					Go_Left(_float fTimeDelta);
	void					Go_Right(_float fTimeDelta);

	void					Turn(_fvector vAxis, _float fTimeDelta);
	void					Rotation(_fvector vAxis, _float fRadian);
	void					Rotation(_float fRadianX, _float fRadianY, _float fRadianZ);

	void					LookAt(_fvector vAt);

private:
	class CGameObject*		m_pOwner = nullptr;

	_float					m_fSpeedPerSec = {};
	_float					m_fRotationPerSec = {};
	_float4x4				m_WorldMatrix{};

public:
	static CTransform*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END