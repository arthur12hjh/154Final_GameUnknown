#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CCollider;
class CGameObject;
NS_END

NS_BEGIN(Client)
class CAttackHitBox final : public CGameObject
{
public:
	typedef struct AttackHitBox : public GAMEOBJECT_DESC
	{
		//이거 통일해서 넘겨줄거임
		CGameObject*	pAttacker;
		const void*		pData;

		_float3			vImapctDir = {};
		_float			fImpactForce = {};
		COLLIDER		eColType;

		// Collider Collision Type
		HIT_TYPE		eHitBoxType;

		// Collider True Object Type
		HIT_TYPE		eHitObjectType;
	}HIT_BOX_DESC;

private:
	CAttackHitBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAttackHitBox(const CAttackHitBox& Prototype);
	virtual ~CAttackHitBox() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;

private :
	CCollider*					m_pColliderCom = { nullptr };
	_float2						m_vAliveTime = {0.f, 0.3f};

	CGameObject*				m_pAttacker = { nullptr };
	const void*					m_pData = { nullptr };

	_float3						m_vImpactDir = {};
	_float						m_fImpactForce = {};

#ifdef _DEBUG
	_bool						m_bIsDelayDead = { false };
	_float2						m_vDelayDead = {};
#endif // _DEBUG

private :
	HRESULT						Ready_Components(const HIT_BOX_DESC& pDesc);

	void						Begin_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);
	void						OverlappingEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);
	void						End_OverlapEvent(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);

public:
	static	CAttackHitBox*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END