#pragma once

#include "Client_Defines.h"
#include "StaticMap.h"

NS_BEGIN(Engine)

NS_END

NS_BEGIN(Client)

class CGiwajip final : public CStaticMap
{
private:
	CGiwajip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGiwajip(const CGiwajip& Prototype);
	virtual ~CGiwajip() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGiwajip* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CStaticMap* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END