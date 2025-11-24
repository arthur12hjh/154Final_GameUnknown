#pragma once

#include "Client_Defines.h"
#include "StaticMap.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CTombStone final : public CStaticMap
{
private:
	CTombStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTombStone(const CTombStone& Prototype);
	virtual ~CTombStone() = default;

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
	static CTombStone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CStaticMap* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END