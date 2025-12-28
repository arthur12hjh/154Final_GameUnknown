#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CCollider;
class CModel;
class CRigidBody;
NS_END

NS_BEGIN(Client)
class CVirtual_Wall : public CGameObject
{
private:
	CVirtual_Wall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVirtual_Wall(const CVirtual_Wall& Prototype);
	virtual ~CVirtual_Wall() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;

	virtual HRESULT			Render() override;

private:
	class CCollider* m_pCollider = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };

private:
	HRESULT					Ready_Components();

public:
	static CVirtual_Wall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END