#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Client)
class CNaytibaLeftWeaponPart final : public CPartObject
{
public:
	typedef struct tagWeapon_Desc : public CPartObject::PARTOBJECT_DESC
	{
		WCHAR				szWeaponModelPrototype[256];
		const _float4x4*	pSocketMatrix = { nullptr };
	}WEAPON_DESC;

private:
	CNaytibaLeftWeaponPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNaytibaLeftWeaponPart(const CNaytibaLeftWeaponPart& Prototype);
	virtual ~CNaytibaLeftWeaponPart() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;

	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

	virtual HRESULT						Render() override;
	virtual HRESULT						Render_Shadow() override;

	virtual void						Active_SFX(const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference) override;
	virtual void						Activate_PartObject_Collider(const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef);

	void								EnableCollider(_bool bIsEnable);

private:
	const _float4x4*					m_pSocketMatrix = { nullptr };

private:
	HRESULT								Ready_Components(const WEAPON_DESC& Desc);
	HRESULT								Bind_ShaderResources();

public:
	static CNaytibaLeftWeaponPart*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END