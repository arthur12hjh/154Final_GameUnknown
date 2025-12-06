#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CComputeShader;

typedef struct ConstantBuffer_Frustom
{
	_float4		vFrustomPlane[6];
	_float		fDistance;
	_int		iNumInstance;
	_int		iPadding;
	_int		iPadding2;
}CONSTANT_BUFFER_FRUSTOM;

class CFrustum final : public CBase
{
private:
	CFrustum(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Update();

#ifdef _DEBUG
	void						FrustomRender();
#endif

public:
	void						Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse);
	_bool						isIn_WorldFrustum(_fvector vWorldPos, _float fRange);
	_bool						isIn_WorldFrustum(class CCollider* pCollider);

	_bool						isIn_LocalFrustum(_fvector vLocalPos, _float fRange);
	_bool						isIn_DistanceFrustum(_float3 vPoint, _float fDistance);

private:
	_float4						m_vOriginalPoints[8] = {};
	_float4						m_vWorldPoints[8] = {};

	_float4						m_vWorldPlanes[6] = {};
	_float4						m_vLocalPlanes[6] = {};
	_float3						m_vCamPos = {};

	BoundingFrustum*			m_OrizinBoundingFrustom = {};
	BoundingFrustum				m_BoundingFrustom = {};

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

#ifdef _DEBUG
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*									m_pEffect = { nullptr };
	ID3D11InputLayout*								m_pInputLayout = { nullptr };
#endif // _DEBUG

private:
	void				Make_Planes(const _float4* pPoints, _float4* pPlanes);

public:
	static CFrustum*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void		Free() override;
};

NS_END