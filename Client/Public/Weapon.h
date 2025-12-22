#pragma once

#include "Client_Defines.h"
#include "Player_Parts.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Client)

class CWeapon final : public CPlayer_Parts
{
public:
	typedef struct tagWeapon_Desc : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr };
	}WEAPON_DESC;
private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;
	virtual HRESULT				Render_Shadow() override;
	virtual HRESULT				Render_MotionBlur() override;

	virtual void				Active_SFX(const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference) override;
	virtual void				Activate_PartObject_Collider(const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef);

	void						EnableCollider(_bool bIsEnable);

	void						Set_PlayerDesc(PLAYER_DESC* pPlayerDesc);

private:
	CCollider*					m_pColliderCom = { nullptr };

	class CTrailEffect*			m_pTrail[4] = {nullptr};
	class CEffect*				m_pBlood = { nullptr };
	class CEffect*				m_pGigasSpark = { nullptr };
	class CEffect*				m_pCharge = { nullptr };
	_uint						m_iTrail = {};
	TCHAR						m_szRotationAngle[MAX_PATH] = {};
	_float3						m_vRotationQuaternion;
	PLAYER_DESC*				m_pPlayerDesc = { nullptr };

	_float						m_fTrailTime = -10.f;
	_float						m_fChargeTime = -10.f;
	_bool						m_bIsTrail = FALSE;
	_bool						m_bisBlood = FALSE;
	_bool						m_bisGigasSpark = FALSE;

	_bool						m_bIsEnableCollider = { false };

private:
	const _float4x4*			m_pSocketMatrix = { nullptr };
	
private:
	HRESULT						Ready_Components();
	HRESULT						Bind_ShaderResources();
	void						Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);
	void						OverlappingEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);

	_bool						isVisible(); 
public:
	static CWeapon*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END