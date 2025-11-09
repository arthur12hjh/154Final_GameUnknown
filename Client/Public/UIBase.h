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

	UIBASE_DESC* Get_UIBase_Desc() { return m_pUIDesc; }
	void Set_UIBase_Desc(UIBASE_DESC* pDesc) {
		m_pUIDesc = pDesc;
	}

protected:
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };

	UIBASE_DESC* m_pUIDesc{ nullptr };
	
protected:
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();

public:
	virtual void Free() override;
};

NS_END