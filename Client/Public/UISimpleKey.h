#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect_Instance;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CUISimpleKey final : public CUIBase
{
private:
	CUISimpleKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUISimpleKey(const CUISimpleKey& Prototype);
	virtual ~CUISimpleKey() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	INTERACTION_STATE Get_InterState() { return m_eInterState; }

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	CVIBuffer_Rect_Instance* m_pVIBaseBufferCom{ nullptr };
	CTexture* m_pKeyTextureCom{ nullptr };

	INTERACTION_STATE m_eInterState{ INTERACTION_STATE::DEFAULT };
	INTERACTION_STATE m_ePrevInterState{ INTERACTION_STATE::END };

public:
	static CUISimpleKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END