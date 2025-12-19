#pragma once

#include "Deferred.h"

NS_BEGIN(Engine)

class CCascadeShadow final : public CDeferred
{
private:
	CCascadeShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCascadeShadow() = default;

public:
	_float* Get_CascadeEnds() { return m_fCascadeEnds; }

public:
	virtual HRESULT Initialize() override;
	void	Update(_float fTimeDelta);
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer) override;

	//얘는 캐스케이드만 추가
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override;
	void* Get_Desc() override { return &m_tShadowLightDesc; }
public:
	//캐스케이드 그림자용 리소스 바인딩
	HRESULT Bind_Shader_Resource(class CShader* pShader, const _char* pConstantName, D3DTS eType);
	//캐스케이드 그림자용 리소스 바인딩2
	HRESULT Bind_CascadeEnds(class CShader* pShader, const _char* pConstantName, const _char* pConstantName2);
	//캐스케이드 그림자 바인딩
	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override;

public:
	HRESULT Ready_Shadow_Light(const CASCADE_SHADOW_DESC& Desc);

#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override;
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
#endif

private:
	//캐스케이드 관련 정보들
	ID3D11DepthStencilView* m_pCascadeShadowDSV = { nullptr };
	_uint2					m_vCascadeShadowMapSize = { 2048, 2048 };

	_float					m_fCascadeEnds[CASCADE_LEVEL + 1];
	_float4					m_vCascadeNearCorner[CASCADE_LEVEL][4];
	_float4					m_vCascadeFarCorner[CASCADE_LEVEL][4];
	//캐스케이드 갯수만큼 들고 있게 처리.
	_float4x4				m_CasCadeTransformationViewMatrices[CASCADE_LEVEL] = {};
	_float4x4				m_CasCadeTransformationProjMatrices[CASCADE_LEVEL] = {};
	_float4					m_vCascadePositions[CASCADE_LEVEL] = {};

	CASCADE_SHADOW_DESC		m_tShadowLightDesc = {};
	list<class CGameObject*> m_CascadeShadowObjects = {};

private:
	void Seperate_CascadeFrustum();
	void Calc_CascadeMatrices();

private:
	HRESULT Ready_RenderTargets();
	//Cascade랑 Static Shadow는 분리한다.
	HRESULT Ready_CascadeShadowDSVs(_uint iSizeX, _uint iSizeY, _uint iCSMLevel);

public:
	static CCascadeShadow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
