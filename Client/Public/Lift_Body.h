#pragma once

#include "Client_Defines.h"
#include "ClientStruct.h"

#include "Actor.h"

NS_BEGIN(Engine)
class CModel;
class CRigidBody;
NS_END

NS_BEGIN(Client)

class CLift_Body final : public CActor
{
private:
	CLift_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLift_Body(const CLift_Body& Prototype);
	virtual ~CLift_Body() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private :
	CModel* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CLift_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END