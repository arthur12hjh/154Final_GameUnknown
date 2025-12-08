#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect_Instance;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CUIMonsterStaminaFX final : public CUIBase
{
private:
	CUIMonsterStaminaFX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIMonsterStaminaFX(const CUIMonsterStaminaFX& Prototype);
	virtual ~CUIMonsterStaminaFX() = default;

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
	CVIBuffer_Rect_Instance* m_pVIBaseBufferCom{ nullptr };
	CTexture* m_pBaseTextureCom{ nullptr };

	_uint m_iCurrentStamina{ 0 };
	_uint m_iPrevStamina{ 0 };

public:
	static CUIMonsterStaminaFX* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END