#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CUIScript final : public CUIBase
{
private:
	CUIScript(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIScript(const CUIScript& Prototype);
	virtual ~CUIScript() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT RenderText();

	void Set_Script(vector<SCRIPT_DATA> Scripts) { m_Scripts = Scripts; }

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	vector<SCRIPT_DATA>	m_Scripts{};

	_float m_fTimeAcc{ 0.f };

	_uint m_iScriptIdx = 0; 

public:
	static CUIScript* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END