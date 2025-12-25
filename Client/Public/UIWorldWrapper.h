#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CInteraction_Component;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CUIWorldWrapper final : public CUIBase
{
private:
	CUIWorldWrapper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIWorldWrapper(const CUIWorldWrapper& Prototype);
	virtual ~CUIWorldWrapper() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	CInteraction_Component* Get_InteractionCom() { return m_pInteractionCom; }
	void Set_InteractionCom(CInteraction_Component* pInterCom) { m_pInteractionCom = pInterCom; }

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;

	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

	CInteraction_Component* m_pInteractionCom{ nullptr };

public:
	static CUIWorldWrapper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END