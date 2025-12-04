#pragma once

#include "Client_Defines.h"
#include "DesertObject.h"
#include "ClientStruct.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CIron_Floor final : public CDesertObject
{
private:
	CIron_Floor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIron_Floor(const CIron_Floor& Prototype);
	virtual ~CIron_Floor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CIron_Floor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CDesertObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END