#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)

class CUIPopup final : public CUIBase
{
private:
	CUIPopup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPopup(const CUIPopup& Prototype);
	virtual ~CUIPopup() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Open_Popup();
	void Close_Popup(_bool isLevelChange = false, _bool isTeleport = false);

	_bool Get_IsOpen() const { return m_isOpen; }

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;

	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	CTexture* m_pDimTextureCom{ nullptr };

	_float m_fAmount{ 0.f };

	_bool m_isOpen{ false };
	_bool m_isClosing{ false };
	_bool m_isLevelChange{ false };
	_bool m_isTeleport{ false };

public:
	static CUIPopup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END