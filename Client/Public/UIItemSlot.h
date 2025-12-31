#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)
class CUIInstanceBuffer;
class CMousePointer;
class CPlayer;

class CUIItemSlot final : public CUIBase
{
private:
	enum class BTN_STATE { DEFAULT, HOVER, CLICK, SELECT };

	typedef struct tagItemSlotDesc : public SHOP_DESC
	{
		_bool bSoldOut{ false };
	}ITEM_SLOT_DESC;

private:
	CUIItemSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIItemSlot(const CUIItemSlot& Prototype);
	virtual ~CUIItemSlot() = default;

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
	CUIInstanceBuffer*	m_pUIItemSlotBufferCom{ nullptr };
	CUIInstanceBuffer*	m_pUIItemIconBufferCom{ nullptr };
	CTexture*			m_pItemIconsTextureCom{ nullptr };
	CTexture*			m_pItemLockTextureCom{ nullptr };

	CMousePointer*		m_pMousePointer{ nullptr };

	vector<VTX_INSTANCE_DESC>   m_ItemSlotInstances{};
	vector<VTX_INSTANCE_DESC>   m_ItemIconInstances{};
	vector<ITEM_SLOT_DESC>		m_ItemSlotDescs{};

	_float						m_fInteractionTime{ 0.f };		

	INTERACTION_STATE			m_eState;

	_int						m_iCurrentItemIdx{ -1 };
	_int						m_iPrevItemIdx{ -1 };

private:
	HRESULT SetUp_ItemSlot(_float fTimeDelta);
	HRESULT SetUp_ItemIcon();

	HRESULT ItemIcon_Render();
	HRESULT Bind_ItemIcon_ShaderResources();

	_bool MouseEnter(_float2 vPos, _float2 vSize);
	void MouseAction(_float fTimeDelta);

	void Apply_Item(CPlayer* pPlayer);

	HRESULT Render_Text();

public:
	static CUIItemSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END