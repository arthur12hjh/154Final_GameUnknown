#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)
class CUIInstanceBuffer;

class CUIMapSelector final : public CUIBase
{
private:
	enum class BTN_STATE { DEFAULT, HOVER, CLICK, SELECT };

	typedef struct tagMapSelectorDesc : public TRANSPORT_DESC
	{
		_float2 vPos;
		_float2 vSize;
		BTN_STATE eState;
	}MAP_SELECTOR_DESC;

private:
	CUIMapSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIMapSelector(const CUIMapSelector& Prototype);
	virtual ~CUIMapSelector() = default;

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
	vector<VTX_INSTANCE_DESC>   m_ButtonInstances{};
	vector<MAP_SELECTOR_DESC>   m_ButtonDescs{};

private:
	void Set_Button_Info(vector<_int> iTransportDatas);

	HRESULT SetUp_Buttons();

	HRESULT Render_Text();

	_bool MouseEnter(_float2 vPos, _float2 vSize);

public:
	static CUIMapSelector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END