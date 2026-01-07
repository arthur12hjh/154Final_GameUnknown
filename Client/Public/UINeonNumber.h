#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)
class CUIInstanceBuffer;

class CUINeonNumber final : public CUIBase
{
private:
	CUINeonNumber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUINeonNumber(const CUINeonNumber& Prototype);
	virtual ~CUINeonNumber() = default;

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
	CUIInstanceBuffer* m_pUINeonNumberBufferCom{ nullptr };
	CTexture* m_pNeonNumberTextureCom{ nullptr };

	vector<VTX_INSTANCE_DESC>   m_NeonNumberInstances{};

	_uint m_iAccuracy = 0;

	_uint m_iCombo = 0;
	_uint m_iPrevCombo = 0;

	_uint m_iHighCombo = 0;

	_float m_fScaleRatio = 1.f;

	_float m_fAlpha = 1.f;
	_float m_fTimeAcc = 0.f;

private:
	HRESULT Render_NeonNumber();
	HRESULT SetUp_NeonNumber();
	HRESULT Bind_NeonNumberShaderResources();

public:
	static CUINeonNumber* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END