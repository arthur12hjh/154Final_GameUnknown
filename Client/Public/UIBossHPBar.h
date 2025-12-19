#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CUIBossHPBar final : public CUIBase
{
private:
	CUIBossHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIBossHPBar(const CUIBossHPBar& Prototype);
	virtual ~CUIBossHPBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_float Get_CurrentFill() const { return m_fCurrentFill; }

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	LONGLONG* m_iMaxHp = nullptr;        // 최대 값
	LONGLONG* m_iCurrentHp = nullptr;        // 최대 값

	_float m_fCurrentFill = 1.f;   // 현재 값
	_float m_fTargetFill = 1.f;    // 목표값
	_float m_fSpeed = 5.0f;         // 빠르게 감소시킬지

public:
	static CUIBossHPBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END