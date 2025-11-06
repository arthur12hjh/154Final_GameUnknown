#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CCollider abstract : public CComponent 
{
public:
	typedef struct CColliderDesc
	{
		_float3				vCenter;
	}COLLIDER_DESC;

	typedef struct DefaultHitDesc
	{
		_float3					vHitPoint;
		_float					vfDistance;
		_float3					vDireaction;
		_float3					vNormal;
	}DEFAULT_HIT_DESC;

protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* pArg);
	virtual void			UpdateColiision(_matrix WorldMatrix);
	
	virtual _bool			Intersect(COLLIDER eType, CCollider* pTarget) = 0;
	virtual _bool			RayIntersect(COLLIDER eType, CCollider* pTarget, DEFAULT_HIT_DESC& OutDesc) { return false; }
	virtual _bool			RayHit(_vector vOrizin, _vector vDiraction, DEFAULT_HIT_DESC& OutDesc);
	virtual	ContainmentType	Contains(_vector Point);
	
	virtual HRESULT			Render() override;

	void					BindBeginOverlapEvent(function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> BeginEvent);
	void					BindOverlappingEvent(function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> OverlappingEvent);
	void					BindEndOverlapEvent(function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> EndEvent);

	void					ADD_HitObejct(CGameObject* pObject);
	void					ADD_IgnoreObejct(HIT_TYPE typeID);
	void					ADD_OnlyHitObject(HIT_TYPE typeID);

	void					CallFunction();
	void					ResetCollision();

	const COLLIDER&			GetCollisionType() const { return m_CollisionType; }
	const HIT_TYPE&			GetCollisionHitType() const { return m_CollisionHitType; }

protected:
	COLLIDER					m_CollisionType = {};
	HIT_TYPE					m_CollisionHitType = {};
	
	list<CGameObject*>			m_OldHitList = {};
	list<CGameObject*>			m_HitList = {};

	HIT_TYPE					m_eOnlyHitType = { HIT_TYPE::END };
	DEFAULT_HIT_DESC			m_HitDesc = {};

	set<HIT_TYPE>				m_IgnoreObject;
	_bool						m_bIsHit = false;

	function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> m_BeginHitFunc = nullptr;
	function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> m_OverlapHitFunc = nullptr;
	function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> m_EndHitFunc = nullptr;

#ifdef _DEBUG
	PrimitiveBatch<DirectX::VertexPositionColor>*		m_pBatch = { nullptr };
	BasicEffect*										m_pEffect = { nullptr };
	ID3D11InputLayout*									m_pInputLayout = { nullptr };
#endif // _DEBUG

protected :
	_bool						IntersectAble(HIT_TYPE TagetTypeHashCode);

public :
	virtual		CComponent*		Clone(void* pArg) override;	
	virtual		void			Free()  override;

};
NS_END