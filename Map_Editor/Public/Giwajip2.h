#pragma once

#include "Maptool_Defines.h"
#include "StaticMap.h"

NS_BEGIN(Engine)

NS_END

NS_BEGIN(Tool_Map)

class CGiwajip2 final : public CStaticMap
{
private:
	CGiwajip2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGiwajip2(const CGiwajip2& Prototype);
	virtual ~CGiwajip2() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGiwajip2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CStaticMap* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END