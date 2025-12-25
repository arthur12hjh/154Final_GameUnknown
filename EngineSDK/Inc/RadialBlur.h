#pragma once

#include "Deferred.h"

NS_BEGIN(Engine)

class CRadialBlur final : public CDeferred
{
private:
	CRadialBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRadialBlur() = default;

public:
	void Set_Active(_uint iSampleCount, _float fSamplePower, _float fLifeTime);
	void Update(_float fTimeDelta);
public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override { return S_OK; }
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer) override { return S_OK; }
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer, const _wstring& strRTTag, const _wstring& strReturnRTTag);

	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override { return S_OK; }
#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override { return S_OK; }
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override { return S_OK; }
#endif

private:
	_float m_fSamplePower = { 0.5f }; 
	_uint m_iSampleCount = { 0 };
	_float m_fTimeAcc = { 0.f };
	_float m_fLifeTime = { 1.f };
	_bool  m_isActive = { false };

public:
	static CRadialBlur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END