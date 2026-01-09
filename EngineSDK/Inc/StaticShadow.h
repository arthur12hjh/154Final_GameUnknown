#pragma once

#include "Deferred.h"

NS_BEGIN(Engine)

class CStaticShadow final : public CDeferred
{
private:
	CStaticShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CStaticShadow() = default;

public:
	virtual HRESULT Initialize() override;
	//정적 오브젝트용
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override;
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer) override;
	void Clear_StaticShadowObjects();
public:
	//정적 그림자용 리소스 바인딩
	HRESULT Bind_Shader_Resource(class CShader* pShader, const _char* pConstantName, D3DTS eType);
	//정적 그림자 바인딩
	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override;

public:
	HRESULT Ready_Shadow_Light(const STATIC_SHADOW_DESC& Desc);

#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override;
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
#endif



private:
	//정적 그림자 정보
	_float4x4				 m_StaticTransformationMatrices[ENUM_CLASS(D3DTS::END)] = {};
	ID3D11DepthStencilView*  m_pStaticShadowDSV = { nullptr };
	_uint2					 m_vStaticOrhtoSize = { 1024, 1024 };
	_uint2					 m_vStaticShadowMapSize = { 16384, 16384 };

	STATIC_SHADOW_DESC		 m_tShadowLightDesc = {};
	list<class CGameObject*> m_StaticShadowObjects = {};

private:
	HRESULT Ready_RenderTargets();
	//Cascade랑 Static Shadow는 분리한다.
	HRESULT Ready_StaticShadowDSVs(_uint iSizeX, _uint iSizeY);

public:
	static CStaticShadow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
