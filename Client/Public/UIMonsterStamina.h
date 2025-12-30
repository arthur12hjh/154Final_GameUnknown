#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect_Instance;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUIMonsterStamina final : public CUIBase
{
private:
	CUIMonsterStamina(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIMonsterStamina(const CUIMonsterStamina& Prototype);
	virtual ~CUIMonsterStamina() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_uint Get_CurrentStamina() const { return m_iCurrentStamina; }
	_float Get_CurrentFill() const { return static_cast<_float>(m_iCurrentStamina + 1) / m_iMaxStamina; }

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	CVIBuffer_Rect_Instance* m_pVIBaseBuffer = { nullptr };
	
	_float m_fCurrentFill = 1.f;   // ÇöÀç °ª

	_uint m_iCurrentStamina{ 0 };
	_uint m_iMaxStamina{ 0 };

public:
	static CUIMonsterStamina* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END