#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CUIGetterQueue final : public CUIBase
{
private:
	typedef struct tagQueueDesc
	{
		_float fStart{ 0.f };
		_float fTime{ 0.f };
		_wstring szText;
		_float fAlpha{ 0.f };
		_uint iIdx{ 0 };
		_float fOffsetY{ 0.f };
		_bool bShow{ false };
	}QUEUE_DESC;

private:
	CUIGetterQueue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGetterQueue(const CUIGetterQueue& Prototype);
	virtual ~CUIGetterQueue() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
	void Insert_Queue(_wstring szText);
	void Pop_Queue(_float fTimeDelta);

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;

	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	void Render_Queue_Item(QUEUE_DESC Desc);
	void Render_MixedText(const wstring& text, const _float2& pos, const _vector& color, _float fScale);

	_int GetCharType(WCHAR c);

private:
	vector<QUEUE_DESC> m_QueueItems{};

	_float m_fTimeAcc{ 0.f };
	_int m_iHead{ 0 };
	_int m_iCount{ 0 };

public:
	static CUIGetterQueue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END