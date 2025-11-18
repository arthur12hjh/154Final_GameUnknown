#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CShader;

class CFrustum final : public CBase
{
public :
	typedef struct CascadeDesc
	{
		_float4x4	ViewMatrix[3];
		_float4x4	ProjMatrix[3];
	}CASCADE_DESC;

	typedef struct CascadeDefferdDesc
	{
		_float4x4	VPMatrix[3];
		_float		CasCadeDist[3];
		_float		Padding;
	}CASCADE_DEFFERD_DESC;

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

	void						Bind_CasCadeSRV();
	void						Bind_ShadowMatrix();
	void						Bind_ShadowDefferd();

	ID3D11DepthStencilView*		GetShadowDSV();

private:
	_uint						m_iNumCascadeCount = {};
	vector<_float>				m_CascadeFar;

	_float4						m_vCascadeFrustumConers[8] = {};

	ID3D11Buffer*				m_pCaseCadeCB[2] = {nullptr, nullptr};
	ID3D11ShaderResourceView*	m_pCasCadeSRV = { nullptr };
	ID3D11DepthStencilView*		m_pCasecasdeDSV = { nullptr };

	CASCADE_DESC				m_CaseCadeMatrix = {};
	CASCADE_DEFFERD_DESC		m_CaseCadeDefferdDesc = {};

	_float4				m_vOriginalPoints[8] = {};
	_float4				m_vWorldPoints[8] = {};
	
	_float4				m_vWorldPlanes[6] = {};
	_float4				m_vLocalPlanes[6] = {};

	BoundingFrustum*	m_OrizinBoundingFrustom = {};
	BoundingFrustum		m_BoundingFrustom = {};

private:
	class CGameInstance* m_pGameInstance = { nullptr };

#ifdef _DEBUG
	ID3D11Device*									m_pDevice = { nullptr };
	ID3D11DeviceContext*							m_pContext = { nullptr };

	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*									m_pEffect = { nullptr };
	ID3D11InputLayout*								m_pInputLayout = { nullptr };
#endif // _DEBUG

private:
	void			Make_Planes(const _float4* pPoints, _float4* pPlanes);
	HRESULT			Ready_CasCadeTexture();

public:
	static CFrustum* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END