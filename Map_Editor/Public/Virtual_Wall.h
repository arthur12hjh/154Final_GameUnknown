#pragma once
#include "Maptool_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CCollider;
class CShader;
NS_END

NS_BEGIN(Tool_Map)
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
	class CCollider*		m_pCollider = { nullptr };

private:
	HRESULT					Ready_Components();

public:
	static CVirtual_Wall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END