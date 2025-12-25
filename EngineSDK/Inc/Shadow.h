#pragma once

#include "Deferred.h"

NS_BEGIN(Engine)

class CShadow final : public CDeferred
{
private:
	CShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CShadow() = default;

public:
	_float* Get_CascadeEnds() { return m_fCascadeEnds; }

public:
	virtual HRESULT Initialize() override;
	void Update(_float fTimeDelta);
	HRESULT Render();
	//정적 그림자 굽기
	HRESULT Bake_StaticShadows(); 

	//정적 오브젝트용
	HRESULT Add_StaticShadowObject(class CGameObject* pGameObject);
	//얘는 캐스케이드만 추가
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override;
	//오버라이딩
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer) override { return S_OK; };

public:
	//정적 그림자용 리소스 바인딩
	HRESULT Bind_Shader_Resource_Static(class CShader* pShader, const _char* pConstantName, D3DTS eType);
	//캐스케이드 그림자용 리소스 바인딩
	HRESULT Bind_Shader_Resource_Cascade(class CShader* pShader, const _char* pConstantName, D3DTS eType);
	//캐스케이드 그림자용 리소스 바인딩2
	HRESULT Bind_Cascade_Ends(class CShader* pShader, const _char* pConstantName, const _char* pConstantName2);
	//캐스케이드 그림자 바인딩
	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override;
	//정적 그림자 바인딩
	HRESULT Bind_StaticShadow_RenderTarget(class CShader* pShader, const _char* pConstantName);

public:
	HRESULT Ready_Shadow_Light(const SHADOW_LIGHT_DESC& Desc);

#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override;
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
#endif

private:
	//정적 그림자 정보
	_float4x4				m_StaticTransformationMatrices[ENUM_CLASS(D3DTS::END)] = {};
	ID3D11DepthStencilView* m_pStaticShadowDSV = { nullptr };
	_uint2					m_vStaticShadowMapSize = { 2048, 2048 };

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

	SHADOW_LIGHT_DESC		m_tShadowLightDesc = {};
	list<class CGameObject*> m_StaticShadowObjects = {};
	list<class CGameObject*> m_CascadeShadowObjects = {};

private:
	void Seperate_CascadeFrustum();
	void Calc_CascadeMatrices();

private:
	HRESULT Ready_RenderTargets();
	//Cascade랑 Static Shadow는 분리한다.
	HRESULT Ready_CascadeShadowDSVs(_uint iSizeX, _uint iSizeY, _uint iCSMLevel);
	HRESULT Ready_StaticShadowDSVs(_uint iSizeX, _uint iSizeY);

public:
	static CShadow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END