#pragma once
#include "Maptool_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CCollider;
class CShader;
NS_END

NS_BEGIN(Tool_Map)
class CSpawnBox : public CGameObject
{
private:
	CSpawnBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpawnBox(const CSpawnBox& Prototype);
	virtual ~CSpawnBox() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;

	virtual HRESULT			Render() override;

	void					SetSpawnPrototypeName(const WCHAR* szPrototypeName);
	const WCHAR*			GetSpawnObjectName();

private:
	const WCHAR*			m_szPrototypeName = {};
	class CCollider*		m_pCollider = { nullptr };

private:
	HRESULT					Ready_Components();

public:
	static CSpawnBox*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END