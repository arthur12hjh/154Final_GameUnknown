#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)
class CUIInstanceBuffer;

class CUIResult final : public CUIBase
{
private:
	CUIResult(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIResult(const CUIResult& Prototype);
	virtual ~CUIResult() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Open_Result();
	void Close_Result();

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	CUIInstanceBuffer* m_pUIScoreBufferCom{ nullptr };
	CUIInstanceBuffer* m_pUIComboBufferCom{ nullptr };
	CTexture* m_pComboTextureCom{ nullptr };
	CTexture* m_pRankTextureCom{ nullptr };
	CTexture* m_pScoreTextureCom{ nullptr };
	CTexture* m_pNumberTextureCom{ nullptr };
	CTexture* m_pNeonNumberTextureCom{ nullptr };
	CTexture* m_pThumbnailTextureCom{ nullptr };

	_uint m_iRank = 0;
	_uint m_iCurScore = 0;
	_uint m_iScore = 0;
	_uint m_iCurCombo = 0;
	_uint m_iCombo = 0;

	vector<VTX_INSTANCE_DESC>   m_ScoreInstances{};
	vector<VTX_INSTANCE_DESC>   m_ComboInstances{};

	_bool m_isOpen{ false };
	_bool m_isOpening{ false };
	_bool m_isShowCombo{ false };
	_bool m_isShowScore{ false };
	_bool m_isShowRank{ false };

	_bool m_isPlayComboSound{ false };
	_bool m_isPlayScoreSound{ false };
	_bool m_isPlayRankSound{ false };

	_float2 m_vThumbnailOffset{1600.f, 0.f};

	_float m_fTimeAcc{ 0.f };
	_float m_fGlowPower{ 0.f };
	_float m_fScale{ 5.f };

	_bool m_bActiveSpace{ false };

private:
	HRESULT Render_Text();

	HRESULT Render_Glows();
	HRESULT Bind_GlowShaderResources();

	HRESULT Render_Score();
	HRESULT SetUp_Score();
	HRESULT Bind_ScoreShaderResources();

	HRESULT Render_Combo();
	HRESULT SetUp_Combo();
	HRESULT Bind_ComboShaderResources();


public:
	static CUIResult* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END