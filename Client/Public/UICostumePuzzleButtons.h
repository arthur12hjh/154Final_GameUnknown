#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)
class CUIInstanceBuffer;

class CUICostumePuzzleButtons final : public CUIBase
{
private:
	enum class BTN_STATE { DEFAULT, HOVER, CLICK, SELECT };

private:
	CUICostumePuzzleButtons(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUICostumePuzzleButtons(const CUICostumePuzzleButtons& Prototype);
	virtual ~CUICostumePuzzleButtons() = default;

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
	CUIInstanceBuffer* m_pUIButtonsBufferCom{ nullptr };

	CTexture* m_pSuitIconsTextureCom{ nullptr };
	CTexture* m_pButtonsTextureCom{ nullptr };

	vector<VTX_INSTANCE_DESC>   m_ButtonInstances{};
	vector<_int> m_Buttons{};
	_int m_iButtonIdx = -1;

	BTN_STATE m_eBtnState{ BTN_STATE::DEFAULT };
	BTN_STATE m_ePrevBtnState{ m_eBtnState };

private:
	HRESULT Render_Buttons();

	HRESULT SetUp_Buttons();

	HRESULT Bind_ButtonsResources();

	_bool MouseEnter(_float2 vPos, _float2 vSize);

public:
	static CUICostumePuzzleButtons* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END