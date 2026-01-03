#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)
class CUIInstanceBuffer;

class CUIScore final : public CUIBase
{
private:
	CUIScore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIScore(const CUIScore& Prototype);
	virtual ~CUIScore() = default;

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
	CUIInstanceBuffer* m_pUIScoreBufferCom{ nullptr };
	CTexture* m_pNumbersTextureCom{ nullptr };

	vector<VTX_INSTANCE_DESC>   m_ScoreInstances{};

	_uint m_iScore = 0;
	_uint m_iPrevScore = 0;

	_uint m_iRank = 0;
	_uint m_iPrevRank = 0;

	_float m_fScaleRatio = 1.f;

private:
	HRESULT Render_Score();
	HRESULT SetUp_Score();
	HRESULT Bind_ScoreShaderResources();


public:
	static CUIScore* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END