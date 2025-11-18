#pragma once

#include "Deferred.h"

NS_BEGIN(Engine)

class CBloom final : public CDeferred
{
public:
	enum class SHADER_BLOOM_IDX { SAMPLING, CURVE, SAMPLING_BLUR_X, SAMPLING_BLUR_Y, ADDITIVE_BLUR_X, ADDITIVE_BLUR_Y, COMBINE };
private:
	CBloom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBloom() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override { return S_OK; }
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer) override { return S_OK;  };
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer, const _wstring& strRenderTargetTag, const _wstring& strCombineRTTag);
	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override;
#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override;
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
#endif

private:
	vector<ID3D11DepthStencilView*> m_pDSVs = {};
	_uint m_iBloomLevel = { 2 }; 
	_uint m_iSampleLevel = { 5 };

	_uint2 m_vOriginScreenSize = {};

	_wstring m_strRenderTargetTags[8] = {
		{ TEXT("Target_BloomDownSample") },
		{ TEXT("MRT_BloomDownSample") },
		{ TEXT("Target_BloomUpSample") },
		{ TEXT("MRT_BloomUpSample") },
		{ TEXT("Target_BloomUpSample_BlurX") },
		{ TEXT("MRT_BloomUpSample_BlurX") },
		{ TEXT("Target_BloomUpSample_BlurY") },
		{ TEXT("MRT_BloomUpSample_BlurY") }
	};

	_wstring m_strPreRenderTargetTag = { TEXT("")};

private:
	HRESULT Ready_RenderTargets();
	HRESULT Ready_DSVs();

	HRESULT DownSampling(class CVIBuffer* pVIBuffer, const _wstring& strSceneRenderTargetTag);
	HRESULT MiddleBlur(class CVIBuffer* pVIBuffer);
	HRESULT UpSampling(class CVIBuffer* pVIBuffer);

public:
	static CBloom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END