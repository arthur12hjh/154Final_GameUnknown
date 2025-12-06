#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect_Instance;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CUISimpleKey final : public CUIBase
{
private:
	CUISimpleKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUISimpleKey(const CUISimpleKey& Prototype);
	virtual ~CUISimpleKey() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	SKILL_STATE Get_KeyState() { return m_eKeyState; }

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	CVIBuffer_Rect_Instance* m_pVIBaseBufferCom{ nullptr };
	CTexture* m_pKeyTextureCom{ nullptr };

	SKILL_STATE m_eKeyState{ SKILL_STATE::DEFAULT };
	SKILL_STATE m_ePrevKeyState{ m_eKeyState };

public:
	static CUISimpleKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END