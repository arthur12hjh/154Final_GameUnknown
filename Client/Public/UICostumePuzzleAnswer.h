#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)
class CUIInstanceBuffer;

class CUICostumePuzzleAnswer final : public CUIBase
{
private:
	CUICostumePuzzleAnswer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUICostumePuzzleAnswer(const CUICostumePuzzleAnswer& Prototype);
	virtual ~CUICostumePuzzleAnswer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_isSolved(_bool isSolved) { m_isSolved = isSolved; }

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	CUIInstanceBuffer* m_pUIIconsBufferCom{ nullptr };

	CTexture* m_pSuitIconsTextureCom{ nullptr };

	vector<VTX_INSTANCE_DESC>   m_IconInstances{};
	vector<_int> m_SelectedIndices{};
	vector<_int> m_Answer{};

	_bool m_isAnswer{ false };
	_bool m_isSolved{ false };
	_int m_iSubmitAnswerIdx{ 0 };

private:
	HRESULT Render_Icons();
	HRESULT Render_Text();

	HRESULT SetUp_Icons();

	HRESULT Bind_IconsResources();

	_bool Check_Answer();

public:
	static CUICostumePuzzleAnswer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END