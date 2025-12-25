#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Client)
class CNpcFace final : public CPartObject
{
private:
	CNpcFace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpcFace(const CNpcFace& Prototype);
	virtual ~CNpcFace() = default;

public:
	virtual HRESULT										Initialize_Prototype() override;
	virtual HRESULT										Initialize(void* pArg) override;

	virtual void										Priority_Update(_float fTimeDelta) override;
	virtual void										Update(_float fTimeDelta) override;
	virtual void										Late_Update(_float fTimeDelta) override;

	virtual HRESULT										Render() override;
	virtual HRESULT										Render_Shadow() override;

private:
	HRESULT												Ready_Components();
	HRESULT												Bind_ShaderResources();

public:
	static		CNpcFace*								Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject*							Clone(void* pArg) override;
	virtual		void									Free() override;

};
NS_END