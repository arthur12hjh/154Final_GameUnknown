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

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;

	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	CTexture* m_pDimTextureCom{ nullptr };
	CTexture* m_pInnerFrameTextureCom{ nullptr };
	CTexture* m_pAnswerTextureCom{ nullptr };
	CTexture* m_pSuitIconsTextureCom{ nullptr };

	_float m_fAmount{ 0.f };

public:
	static CUIPopup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END