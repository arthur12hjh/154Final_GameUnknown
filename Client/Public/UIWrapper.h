#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CUIWrapper final : public CUIBase
{
public:
	enum class BTN_STATE { DEFAULT, HOVER, CLICK, SELECT };

private:
	CUIWrapper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIWrapper(const CUIWrapper& Prototype);
	virtual ~CUIWrapper() = default;

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
	_bool MouseEnter();

	BTN_STATE m_eBtnState{ BTN_STATE::DEFAULT };
	BTN_STATE m_ePrevBtnState{ m_eBtnState };

public:
	static CUIWrapper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END