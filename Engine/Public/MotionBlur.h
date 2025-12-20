#pragma once

#include "Deferred.h"

NS_BEGIN(Engine)

class CMotionBlur final : public CDeferred
{
private:
	CMotionBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMotionBlur(const CMotionBlur& rhs);
	virtual ~CMotionBlur() = default;

public:
	virtual void* Get_Desc() override;

public:
	virtual HRESULT	Initialize() override;
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override { return S_OK;  }

	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer) { return S_OK; }
	HRESULT Render(class CVIBuffer_Rect* pVIBuffer, const _wstring& strSceneRTTag, const _wstring& strReturnRTTag);
	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override;

	_float2 Calc_CamVelocity();

#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override;
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
#endif

private:
	MOTIONBLUR_DESC m_Desc = {};
	_float m_fCamBlurScale = { 0.05f };
	_float m_fObjectBlurScale = { 1.45f };
	_float m_fBias = { 0.8f };
	_uint   m_iSampleCount = { 16 };
public:
	static CMotionBlur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
