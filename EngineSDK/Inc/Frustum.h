#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CComputeShader;

typedef struct ConstantBuffer_Frustom
{
	_float4		vFrustomPlane[6];
	_float3		vCamPos;
	_float		fDistance;

	_int		iNumInstance;
	_float3		vPadding;
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
	void						isIn_WorldFrustum(ID3D11Buffer* pInstanceBuffer, ID3D11Buffer* pOut, _uint iNumInstance, _float fDistance = 0.f, _uint* iNumCullCount = nullptr);

private:
	_float4						m_vOriginalPoints[8] = {};
	_float4						m_vWorldPoints[8] = {};
	
	_float4						m_vLocalPlanes[6] = {};

	BoundingFrustum*			m_OrizinBoundingFrustom = {};
	BoundingFrustum				m_BoundingFrustom = {};

	CComputeShader*				m_pComputeShader = { nullptr };
	CONSTANT_BUFFER_FRUSTOM		m_FrustomConstantDesc = {};

	_uint						m_iNumData = { 1024 };

	ID3D11Buffer*				m_pCountBuffer = { nullptr };

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
	HRESULT				Ready_ComputeShader();
	void				Make_Planes(const _float4* pPoints);
	void				Make_Planes(const _float4* pPoints, _float4* pPlanes);
public:
	static CFrustum*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void		Free() override;
};

NS_END