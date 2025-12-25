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
	virtual HRESULT		Render_Shadow() override;

	virtual HRESULT ActiveCinematicObject(const CINEMATIC_NODE_DESC& CinematicNodeDesc);
	virtual HRESULT PlayCinematicObject(const CINEMATIC_NODE_DESC& CinematicNodeDesc);

private:
	class CWeapon* m_pWeapon = { nullptr };

	Player_Desc			m_PlayerDesc = {};

	_float3		m_vRotationQuaternion = {};
	_float		m_fMoveTime = 0.f;
	_int		m_iAnimationSequence = 0;

	_bool		m_bIsPrevActivated = FALSE;

private:
	HRESULT Ready_Components();
	HRESULT Ready_PlayerDesc();
	HRESULT Ready_PartObjects();

	// Cinematic Code
	// 0 : Gorilla Meet
	// 1 : Gorilla Finish
	// 10 : ..
	// 20 : Scarlet Battle Enter
	// 21 : Scarlet Battle Phase Change
	// 22 : Scarlet Battle Finish
	//


	HRESULT Initialize_Cinematic_GorillaMeet();
	HRESULT Initialize_Cinematic_GorillaFinish();

	HRESULT Initialize_Cinematic_Scarlet_Battle_Enter();
	HRESULT Initialize_Cinematic_Scarlet_Battle_PhaseChange();
	HRESULT Initialize_Cinematic_Scarlet_Battle_Finish();

	HRESULT Play_Cinematic_GorillaMeet(_float fTimeDelta);
	HRESULT Play_Cinematic_GorillaFinish(_float fTimeDelta);

	HRESULT Play_Cinematic_Scarlet_Battle_Enter(_float fTimeDelta);
	HRESULT Play_Cinematic_Scarlet_Battle_PhaseChange(_float fTimeDelta);
	HRESULT Play_Cinematic_Scarlet_Battle_Finish(_float fTimeDelta);

	void Reset_CinematicChanges();

public:
	static CCinematicModel_Eve* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END