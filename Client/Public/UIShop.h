#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)

class CUIShop final : public CUIBase
{
private:
	CUIShop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIShop(const CUIShop& Prototype);
	virtual ~CUIShop() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Open_Shop();
	void Close_Shop();

	_bool Get_IsOpen() const { return m_isOpen; }

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;

	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	CTexture* m_pNpcTextureCom{ nullptr };

	_float m_fAmount{ 0.f };

	_bool m_isOpen{ true };
	_bool m_isClosing{ false };
	_bool m_isLevelChange{ false };

public:
	static CUIShop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END