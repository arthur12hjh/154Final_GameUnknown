#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class UIActionEvent;

class CUIBossStamina final : public CUIBase
{
private:
	CUIBossStamina(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIBossStamina(const CUIBossStamina& Prototype);
	virtual ~CUIBossStamina() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
	_uint Get_CurrentStamina() const { return m_iCurrentStamina; }
	_float Get_CurrentFill() const { return static_cast<_float>(m_iCurrentStamina + 1) / m_iMaxStamina; }

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	_float m_fCurrentFill = 1.f;   // ÇöÀç °ª

	_uint m_iCurrentStamina{ 9 };
	_uint m_iMaxStamina{ 9 };

public:
	static CUIBossStamina* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END