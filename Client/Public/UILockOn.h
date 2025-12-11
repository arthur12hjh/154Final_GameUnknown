#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect_Instance;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CUILockOn final : public CUIBase
{
private:
	CUILockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUILockOn(const CUILockOn& Prototype);
	virtual ~CUILockOn() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Finisher(_bool isFinisher) { m_isFinisher = isFinisher; }

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;
	
private:
	CVIBuffer_Rect_Instance* m_pVIBaseBufferCom{ nullptr };
	CTexture* m_pShadowTextureCom{ nullptr };
	CTexture* m_pKeyTextureCom{ nullptr };
	CTexture* m_pRingTextureCom{ nullptr };

	_bool m_isFinisher{ false };
	_float m_fTimeAcc{ 0.f };

public:
	static CUILockOn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END