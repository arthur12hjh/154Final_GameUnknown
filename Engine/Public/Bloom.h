#pragma once

#include "Deferred.h"

/*
ºí·ë (±¸ÇöÁß)
*/
NS_BEGIN(Engine)

class CBloom final : public CDeferred
{
public:
	enum class SHADER_BLOOM_IDX { SAMPLING, CURVE, SAMPLING_BLUR_X, SAMPLING_BLUR_Y, ADDITIVE_BLUR_X, ADDITIVE_BLUR_Y };
private:
	CBloom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBloom() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override { return S_OK; }
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer) override { return S_OK;  };
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer, const _wstring& strRenderTargetTag);
	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override;
#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override;
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
#endif

private:
	ID3D11DepthStencilView* m_p4x4SampleDSV = { nullptr };
	ID3D11DepthStencilView* m_p20x20SampleDSV = { nullptr };

private:
	HRESULT Ready_DSVs();
public:
	static CBloom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END