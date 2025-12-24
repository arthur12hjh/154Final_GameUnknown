#pragma once

#include "CinematicObject.h"
#include "ClientStruct.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Client)
class CWeapon;

class CCinematicModel_Scarlet final : public CCinematicObject
{
private:
	CCinematicModel_Scarlet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCinematicModel_Scarlet(const CCinematicModel_Scarlet& Prototype);
	virtual ~CCinematicModel_Scarlet() = default;

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
	virtual HRESULT		Render_Shadow() override;

	virtual HRESULT ActiveCinematicObject(const CINEMATIC_NODE_DESC& CinematicNodeDesc);
	virtual HRESULT PlayCinematicObject(const CINEMATIC_NODE_DESC& CinematicNodeDesc);

private:
	CPartObject*		m_pWeapon = { nullptr };
	CPartObject*		m_pScabbard = { nullptr };

	_float3		m_vRotationQuaternion = {};
	_float		m_fMoveTime = 0.f;
	_int		m_iAnimationSequence = 0;


private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

	HRESULT Initialize_Cinematic_GorillaMeet();
	HRESULT Initialize_Cinematic_GorillaFinish();

	HRESULT Play_Cinematic_GorillaMeet(_float fTimeDelta);
	HRESULT Play_Cinematic_GorillaFinish(_float fTimeDelta);


public:
	static CCinematicModel_Scarlet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END