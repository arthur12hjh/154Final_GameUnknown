#pragma once
#include "Collider.h"

NS_BEGIN(Engine)
class ENGINE_DLL CSphereCollider : public CCollider
{
public :
	typedef struct SphereColiisionDesc : public COLLIDER_DESC
	{
		_float			fRadius;
	}SPHERE_COLLIDER_DESC;

private :
	CSphereCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSphereCollider(const CSphereCollider& rhs);
	virtual ~CSphereCollider() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);
	virtual void					UpdateColiision(_matrix WorldMatrix) override;
	virtual _bool					Intersect(COLLIDER eType, CCollider* pTarget) override;
	virtual _bool					RayIntersect(COLLIDER eType, CCollider* pTarget, DEFAULT_HIT_DESC& OutDesc) override;
	virtual _bool					RayHit(_vector vOrizin, _vector vDiraction, DEFAULT_HIT_DESC& OutDesc) override;
	virtual	ContainmentType			Contains(_vector vPoint) override;
	
	void							SetCollision(_float3 vCenter, _float fRadius);
	
#ifdef _DEBUG
	virtual HRESULT					Render() override;
#endif // _DBUG

	const 	BoundingSphere&			GetBounding() { return *m_Bounding; }

private :
	BoundingSphere*					m_OriginBounding = nullptr;
	BoundingSphere*					m_Bounding = nullptr;

public:
	static		CSphereCollider*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CComponent*			Clone(void* pArg) override;
	virtual		void				Free()  override;

};
NS_END