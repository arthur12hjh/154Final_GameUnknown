#pragma once

#include "Deferred.h"

NS_BEGIN(Engine)

class CDepthofField final : public CDeferred
{
private:
	CDepthofField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDepthofField() = default;

public:
	void Set_Active();
	virtual void* Get_Desc() override;
public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override { return S_OK; }
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer) override { return S_OK; }
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer, const _wstring& strRTTag, const _wstring& strDepthRTTag, const _wstring& strReturnRTTag);

	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override { return S_OK; }
#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override { return S_OK; }
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override { return S_OK; }
#endif

private:
	DOF_DESC m_Desc = {};
	_bool  m_isActive = { false };
	_float m_fFocusDistance = { 20.f };
	_float m_fMaxRange = { 20.f };
	_float m_fIntensity = { 1.f };

public:
	static CDepthofField* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END