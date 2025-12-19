#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Client)
struct Character_Skill_Desc;

class CBullet abstract : public CPartObject
{
public:
	enum class BULLET_TYPE { PROJECTILE, HITSCAN, END };

	typedef struct Bullet_Desc : public PARTOBJECT_DESC
	{
		const _float4x4*		pSocketMatrix = { nullptr };

		_uint					iSkillID;
		_uint					iHitType;
		_uint					iBulletType;
		_float3					vTargetPoint;
	}BULLET_DESC;

protected:
	CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBullet(const CBullet& Prototype);
	virtual ~CBullet() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;
	virtual void					Shoot_Projectile(_vector vTargetPoint, _float fSpeed);

protected :
	const	_float4x4*				m_pSocketMatrix = { nullptr };
	BULLET_TYPE						m_eBulletType = { BULLET_TYPE::END };

	CModel*							m_pModelCom = { nullptr };
	CCollider*						m_pColliderCom = { nullptr };
	const Character_Skill_Desc*		m_pSkillData = { nullptr };

	_bool							m_bIsEnableCollider = { false };
	_float3							m_vProjectileDir = {};
	_float3							m_vTargetPoint = {};

	_float							m_fSpeed = {};
	_uint2							m_iHitScanFrameIndex = {};
	_bool							m_bIsAttachment = { true };

protected :
	void						Update_BulletCombinedMatrix();


public:
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END