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
	virtual _bool			FrustomIntersect(const BoundingFrustum& Frustom);
	
	virtual	ContainmentType	Contains(_vector Point);

	virtual HRESULT			Render() override;
	virtual HRESULT			Render(_float4 vColor);

	void					SetColliderHitType(HIT_TYPE eHitType);

	void					BindBeginOverlapEvent(function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> BeginEvent);
	void					BindOverlappingEvent(function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> OverlappingEvent);
	void					BindEndOverlapEvent(function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> EndEvent);

	void					ADD_HitObject(CGameObject* pObject);

	void					ADD_IgnoreObjectType(HIT_TYPE eHitType);
	void					Remove_IgnoreObjectType(HIT_TYPE eHitType);

	void					ADD_OnlyHitObjectType(HIT_TYPE eHitType);

	void					Clear_HitObjectTypeList();
	void					CallFunction();
	void					ResetCollision();

	const COLLIDER&			GetCollierType() const { return m_eType; }
	const HIT_TYPE&			GetCollierHitType() const { return m_eHitType; }

	const _float4x4*		Get_WorldMatrixPtr() const { return &m_WorldMatrix; }

protected:
	COLLIDER					m_eType = {}; 
	HIT_TYPE					m_eHitType = {};
	
	list<CGameObject*>			m_OldHitList = {};
	list<CGameObject*>			m_HitList = {};

	HIT_TYPE					m_eOnlyHitType = { HIT_TYPE::END };
	DEFAULT_HIT_DESC			m_HitDesc = {};

	set<HIT_TYPE>				m_IgnoreObject;
	_bool						m_bIsHit = false;

	_float4x4					m_WorldMatrix = { };

	function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> m_BeginHitFunc = nullptr;
	function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> m_OverlapHitFunc = nullptr;
	function<void(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor)> m_EndHitFunc = nullptr;
	
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };

	BasicEffect*										m_pEffect = { nullptr };
	ID3D11InputLayout*									m_pInputLayout = { nullptr };

protected :
	_bool						IntersectAble(HIT_TYPE TagetTypeHashCode);

public :
	virtual		CComponent*		Clone(void* pArg) override;	
	virtual		void			Free()  override;

};
NS_END