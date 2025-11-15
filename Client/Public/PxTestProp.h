#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CRigidBody;
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CPxTestProp final : public CGameObject
{
private:
	CPxTestProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPxTestProp(const CPxTestProp& Prototype);
	virtual ~CPxTestProp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRigidBody* m_pRigidBody = { nullptr };
	CModel*  m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPxTestProp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END