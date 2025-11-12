#pragma once

#include "Deferred.h"

NS_BEGIN(Engine)

class CBlur final : public CDeferred
{
private:
	CBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBlur() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override;
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override;
#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override;
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
#endif

private:
	list<class CGameObject*> m_BlurObjects = {};

public:
	static CBlur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END