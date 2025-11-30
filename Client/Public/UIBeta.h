#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect_Instance;
class CTexture;
NS_END

NS_BEGIN(Client)

class UIActionEvent;

class CUIBeta final : public CUIBase
{
private:
	CUIBeta(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIBeta(const CUIBeta& Prototype);
	virtual ~CUIBeta() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Render_Glow();

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	HRESULT Bind_GlowShaderResources();

	CVIBuffer_Rect_Instance* m_pVIBaseBuffer = { nullptr };
	CVIBuffer_Rect_Instance* m_pVIGlowBufferCom = { nullptr };
	
	CTexture* m_pFXTexture{ nullptr };

	_float m_fCurrentFill = 0.5f;   // 현재 값
	_float m_fTargetFill = 0.5f;    // 목표값
	_float m_fSpeed = 5.0f;         // 빠르게 감소시킬지

	_int m_iPrevGroupFilled = -1;
	_int m_iPerCount = 4;

#ifdef _DEBUG
	LONGLONG m_Gara = 20;
	LONGLONG m_MaxGara = 20;
#endif // DEBUG

	LONGLONG* m_iMaxCount = nullptr;
	LONGLONG* m_iCurrentCount = nullptr;

public:
	static CUIBeta* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END