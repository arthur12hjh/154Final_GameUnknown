#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CUIUnlockFX final : public CUIBase
{
private:
	CUIUnlockFX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIUnlockFX(const CUIUnlockFX& Prototype);
	virtual ~CUIUnlockFX() = default;

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
	_bool m_bPlay = false;
	_uint m_iTextureIdx = 0;
	_float m_fTimeAcc = 0.f;

public:
	static CUIUnlockFX* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END