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

	virtual HRESULT Set_Cinematic_Object(const ANIM_NOTIFY& NotifyReference) override;


private:
	CPartObject*					m_pWeapon = { nullptr };
	CPartObject*					m_pScabbard = { nullptr };
	class CCinematicPartBody*		m_pBottle = { nullptr };
	class CCinematicPartBody*		m_pGlass = { nullptr };

	_float3		m_vRotationQuaternion = {};
	_float		m_fMoveTime = 0.f;
	_int		m_iAnimationSequence = 0;

	_bool		m_bIsPrevActivated = FALSE;

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

	HRESULT Initialize_Cinematic_Scarlet_FirstMeet();
	
	HRESULT Initialize_Cinematic_Scarlet_GoodBye();

	HRESULT Initialize_Cinematic_Scarlet_Battle_Enter();
	HRESULT Initialize_Cinematic_Scarlet_Battle_PhaseChange();
	HRESULT Initialize_Cinematic_Scarlet_Battle_Finish();

	HRESULT Play_Cinematic_Scarlet_FirstMeet(_float fTimeDelta);

	HRESULT Play_Cinematic_Scarlet_GoodBye(_float fTimeDelta);

	HRESULT Play_Cinematic_Scarlet_Battle_Enter(_float fTimeDelta);
	HRESULT Play_Cinematic_Scarlet_Battle_PhaseChange(_float fTimeDelta);
	HRESULT Play_Cinematic_Scarlet_Battle_Finish(_float fTimeDelta);

	void Reset_CinematicChanges();


public:
	static CCinematicModel_Scarlet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END