#pragma once

#include "Deferred.h"

/*
ºí·ë (±¸ÇöÁß)
*/
NS_BEGIN(Engine)

class CBloom final : public CDeferred
{
private:
	CBloom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBloom() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override;
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer) override;
	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override;
#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override;
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
#endif

private:

public:
	static CBloom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END