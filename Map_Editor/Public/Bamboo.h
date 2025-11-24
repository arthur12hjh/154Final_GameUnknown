#pragma once

#include "Maptool_Defines.h"
#include "StaticMap.h"

NS_BEGIN(Engine)

NS_END

NS_BEGIN(Tool_Map)

class CBamboo final : public CStaticMap
{
private:
	CBamboo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBamboo(const CBamboo& Prototype);
	virtual ~CBamboo() = default;

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
	static CBamboo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CStaticMap* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END