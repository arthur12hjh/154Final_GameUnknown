#pragma once

#include "Deferred.h"

/* 당장 인자로 뺼 필요는 없어보임.. */
NS_BEGIN(Engine)

class CDistortion final : public CDeferred
{
private:
	CDistortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDistortion() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override;
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer) override { return S_OK; }
	HRESULT Render(class CVIBuffer_Rect* pVIBuffer, const _wstring& strRTTag, const _wstring& strReturnRTTag);
	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override;
#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override;
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
#endif

private:
	list<class CGameObject*> m_DistortionObjects = {};

public:
	static CDistortion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END