#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect_Instance;
NS_END

NS_BEGIN(Client)

class CUIOwnGold final : public CUIBase
{
private:
	CUIOwnGold(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIOwnGold(const CUIOwnGold& Prototype);
	virtual ~CUIOwnGold() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT RenderText();

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	CVIBuffer_Rect_Instance* m_pVIBaseBufferCom{ nullptr };
	CTexture* m_pShadowTextureCom{ nullptr };

	_int* m_iOwnGold{ nullptr };
	_int m_iPrevOwnGold{ 0 };

	_float m_fTimeAcc{ 0.f };
	_float2 m_vTransOffset{ 0.f, 0.f };
	
	_bool m_bShow = false;

public:
	static CUIOwnGold* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END