#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

/*
	hover, selected는 버튼이면 무조건 갖게 해야함
*/

class CUIButton final : public CUIBase
{
public:
	enum class BTN_STATE { DEFAULT, HOVER, CLICK, SELECT };

private:
	CUIButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIButton(const CUIButton& Prototype);
	virtual ~CUIButton() = default;

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

private:
	_bool MouseEnter();

	BTN_STATE m_eBtnState{BTN_STATE::DEFAULT};
	BTN_STATE m_ePrevBtnState{ m_eBtnState };

public:
	static CUIButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END