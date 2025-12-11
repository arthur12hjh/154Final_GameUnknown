#pragma once

#include "Deferred.h"

NS_BEGIN(Engine)

class CFog final : public CDeferred
{
private:
	CFog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFog() = default;

public:
	_float4 Get_FogColor() { return m_vFogColor; }
	virtual void* Get_Desc() override;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override { return S_OK; };
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer) override;
	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override;
#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override;
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
#endif

private:
	FOG_DESC m_Desc = {};
	_float4 m_vFogColor = { 1.0f, 0.89f, 0.70f, 1.f };
	_float  m_fFogStart = { 0.f};
	_float  m_fFogEnd	= { 500.f };
	_float  m_fFogPowerMin = { 0.5f };
	_float	m_fFogPowerMax = { 1.f };
	_float  m_fSkyBoxFogPower = { 0.77f };

public:
	static CFog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END