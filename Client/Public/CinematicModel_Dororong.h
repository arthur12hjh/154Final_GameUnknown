#pragma once

#include "CinematicObject.h"
#include "ClientStruct.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Client)
class CWeapon;

class CCinematicModel_Dororong final : public CCinematicObject
{
private:
	CCinematicModel_Dororong(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCinematicModel_Dororong(const CCinematicModel_Dororong& Prototype);
	virtual ~CCinematicModel_Dororong() = default;

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
	_float3		m_vRotationQuaternion = {};
	_float		m_fMoveTime = 0.f;
	_int		m_iAnimationSequence = 0;

	_bool		m_bIsPrevActivated = FALSE;

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

	HRESULT Initialize_Cinematic_Dororong_Meet();

	HRESULT Play_Cinematic_Dororong_Meet(_float fTimeDelta);

	void Reset_CinematicChanges();


public:
	static CCinematicModel_Dororong* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END