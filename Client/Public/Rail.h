#pragma once
#include "Client_Defines.h"
#include "Actor.h"

NS_BEGIN(Engine)

NS_END

NS_BEGIN(Client)
class CRail : public CActor
{
private:
	CRail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRail(const CRail& Prototype);
	virtual ~CRail() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;

	virtual HRESULT			Render() override;

private:
	HRESULT					Ready_Components();
	HRESULT					Bind_ShaderResources();

private:
	_float					m_fColorWeight = { 0.f };
	_float					m_fTimeAcc = { 0.f };
	_bool					m_bIsColorChange = { false };

public:
	static CRail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END