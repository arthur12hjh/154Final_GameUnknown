#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CUIScript final : public CUIBase
{
	typedef struct tagScriptAnimDesc
	{
		_float fAlpha{ 1.f };
		_float2 vOffset{ 0.f, 0.f };
	}SCRIPT_ANIM_DESC;

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
	HRESULT RenderSpeaker();
	HRESULT RenderScript();

	void Begin_Script(const SCRIPT_DESC* Scripts) {
		m_pScriptDesc = Scripts;
		m_bHasScript = true;
		m_tUIDesc.fAlpha = 1.f;
		m_iScriptIdx = 0;
		m_eVisibility = VISIBILITY::VISIBLE;
	}

	void End_Script() {
		m_pScriptDesc = nullptr;
		m_bHasScript = false;
		m_tUIDesc.fAlpha = 0.f;
		m_iScriptIdx = 0;
		m_eVisibility = VISIBILITY::HIDDEN;
	}

	_bool Get_Has_Script_Desc() const { return m_bHasScript; }

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	const SCRIPT_DESC*		m_pScriptDesc{nullptr};

	_float m_fTimeAcc{ 0.f };
	_uint m_iScriptIdx = 0;
	_bool m_bHasScript = false;

	SCRIPT_ANIM_DESC m_tScriptAnimDesc{};

private:
	void DefaultAnim(_float fTimeDelta);
	void LoadingAnim(_float fTimeDelta);
	void ScriptControl();

public:
	static CUIScript* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END