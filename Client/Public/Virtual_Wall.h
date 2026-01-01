#pragma once
#include "Client_Defines.h"
#include "Actor.h"

NS_BEGIN(Engine)
class CCollider;
class CModel;
NS_END

NS_BEGIN(Client)
class CVirtual_Wall : public CActor
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
	CCollider* m_pCollider = { nullptr };

	_bool		m_bIsReady = { false };

private:
	HRESULT					Ready_Components();

public:
	static CVirtual_Wall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END