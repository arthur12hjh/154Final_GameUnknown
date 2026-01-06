#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)

class CUITriggerKey final : public CUIBase
{
private:
	CUITriggerKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUITriggerKey(const CUITriggerKey& Prototype);
	virtual ~CUITriggerKey() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	CTexture* m_pHoldGaugeTextureCom{ nullptr };

	INTERACTION_STATE m_eInterState{ INTERACTION_STATE::END };
	INTERACTION_STATE m_ePrevInterState{ INTERACTION_STATE::END };

	_bool m_isActive = false;

	_float m_fInteractionTime = 0.f;

	_wstring szActionTag{};

public:
	static CUITriggerKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END