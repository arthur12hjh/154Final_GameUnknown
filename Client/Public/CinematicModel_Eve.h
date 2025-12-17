#pragma once

#include "CinematicObject.h"
#include "ClientStruct.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Client)
class CWeapon;

class CCinematicModel_Eve final : public CCinematicObject
{
private:
	CCinematicModel_Eve(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCinematicModel_Eve(const CCinematicModel_Eve& Prototype);
	virtual ~CCinematicModel_Eve() = default;

public:
	virtual void	 Active_SFX(const _wstring& strPartTag, const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)override;
	virtual void	 Activate_PartObject_Collider(const _wstring& strPartTag, const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef);
	virtual HRESULT	 CallNotify(_uint iNotiType, const AnimNotify* pNotify);

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;

	virtual void		Priority_Update(_float fTimeDelta) override;
	virtual void		Update(_float fTimeDelta) override;
	virtual void		Late_Update(_float fTimeDelta) override;

	virtual HRESULT		Render() override;
	
private:
	CCollider* m_pColliderCom = { nullptr };
	CWeapon* m_pWeapon = { nullptr };
	
private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();


public:
	static CCinematicModel_Eve* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END