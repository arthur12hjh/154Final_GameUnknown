#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)

class CUIBossStaminaFX final : public CUIBase
{
private:
	CUIBossStaminaFX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIBossStaminaFX(const CUIBossStaminaFX& Prototype);
	virtual ~CUIBossStaminaFX() = default;

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
	CTexture* m_pBaseTextureCom{ nullptr };

	_uint m_iCurrentStamina{ 0 };
	_uint m_iPrevStamina{ 0 };

public:
	static CUIBossStaminaFX* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END