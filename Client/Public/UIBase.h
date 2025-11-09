#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CUIBase abstract : public CUIObject
{
public:
	typedef struct tagUIBaseDesc : public CUIObject::UIOBJECT_DESC
	{
		_uint iObjectID{ 0 };
		_uint iLevel{ 0 };
		_uint iDepth{ 0 };
		_uint iPass{ 0 };
		_uint iRenderGroup{ 0 };
		_uint iTextureIndex{ 0 };
		_wstring szLayerTag;
		_wstring szTextureComTag;
	}UIBASE_DESC;

protected:
	CUIBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext); 
	CUIBase(const CUIBase& Prototype);
	virtual ~CUIBase() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	UIBASE_DESC Get_UIBase_Desc() { return m_tUIDesc; }
	void Set_UIBase_Desc(UIBASE_DESC tDesc) {
		m_tUIDesc = tDesc;
	}

	_uint Get_Depth() { return m_tUIDesc.iDepth; }
	void Set_Depth(_uint iDepth) {
		m_tUIDesc.iDepth = iDepth;
	}

	void Set_Parent(CUIBase* pParent) {
		m_pParent = pParent;
	}
	CUIBase* Get_Parent() { return dynamic_cast<CUIBase*>(m_pParent); }

protected:
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };

	UIBASE_DESC m_tUIDesc{};

	//_uint m_iDepth = 0;
	
protected:
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();

public:
	virtual void Free() override;
};

NS_END