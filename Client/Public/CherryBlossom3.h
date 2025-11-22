#pragma once

#include "Client_Defines.h"
#include "StaticMap.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CCherryBlossom3 final : public CStaticMap
{
private:
	CCherryBlossom3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCherryBlossom3(const CCherryBlossom3& Prototype);
	virtual ~CCherryBlossom3() = default;

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
	static CCherryBlossom3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CStaticMap* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END