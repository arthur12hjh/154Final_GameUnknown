#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)

class CUIPotionStack final : public CUIBase
{
private:
	CUIPotionStack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPotionStack(const CUIPotionStack& Prototype);
	virtual ~CUIPotionStack() = default;

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
	_float m_fCurrentFill = 1.0f;   // 현재 값
	_float m_fTargetFill = 1.f;    // 목표값
	_float m_fSpeed = 5.0f;         // 빠르게 감소시킬지
	_float2 m_vPotionUV = { 1.f, 1.f };

public:
	static CUIPotionStack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END