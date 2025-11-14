#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CDeferred abstract : public CBase
{
protected:
	CDeferred(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDeferred() = default;

public:
	virtual HRESULT Initialize() = 0;
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) = 0;
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer) = 0;
	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) = 0;

#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) = 0;
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) = 0;
#endif

protected:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CShader* m_pShader = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	ID3D11Device*		 m_pDevice		 = { nullptr };
public:
	virtual void Free() = 0;
};

NS_END