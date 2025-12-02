#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect_Instance;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUIRushSlot final : public CUIBase
{
private:
	CUIRushSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIRushSlot(const CUIRushSlot& Prototype);
	virtual ~CUIRushSlot() = default;

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
	HRESULT Render_Glow();
	HRESULT Bind_GlowShaderResources();

private:
	SKILL_STATE m_eRushState = SKILL_STATE::DEFAULT;
	_float* m_fCurrentCoolTime = nullptr;
	_float* m_fMaxCoolTime = nullptr;
	_float m_fCoolAmount = 0.f;

	CVIBuffer_Rect_Instance* m_pVIBaseBufferCom = { nullptr };
	CVIBuffer_Rect_Instance* m_pVIGlowBufferCom = { nullptr };
	CTexture* m_pTextureCom2 = { nullptr };
	CTexture* m_pCoolTimeTextureCom = { nullptr };
	CTexture* m_pShadowTextureCom = { nullptr };
	CTexture* m_pGlowTextureCom = { nullptr };
	CTexture* m_pGlowTextureCom2 = { nullptr };

#ifdef _DEBUG
#endif // DEBUG

public:
	static CUIRushSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END