#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)
class CUIInstanceBuffer;

class CUICostumePuzzleHint final : public CUIBase
{
private:
	CUICostumePuzzleHint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUICostumePuzzleHint(const CUICostumePuzzleHint& Prototype);
	virtual ~CUICostumePuzzleHint() = default;

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
	CUIInstanceBuffer* m_pUIIconsBufferCom{ nullptr };

	CTexture* m_pSuitIconsTextureCom{ nullptr };

	vector<VTX_INSTANCE_DESC>   m_IconInstances{};
	vector<_int> m_Answer{};

	_bool m_bPopupOpen{ false };

private:
	HRESULT Render_Icons();
	HRESULT Render_Text();

	HRESULT SetUp_Icons();

	HRESULT Bind_IconsResources();

public:
	static CUICostumePuzzleHint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END