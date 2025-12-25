#pragma once

#include "MapTool_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Tool_Map)

class CLoadingBG final : public CUIObject
{
private:
	CLoadingBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoadingBG(const CLoadingBG& Prototype);
	virtual ~CLoadingBG() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };


	_float m_fLoadingTime = 0.f;
	vector<_int> m_iTextureIndices;
	_uint m_iTextureIndex = 0;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLoadingBG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END