#pragma once
#include "Collider.h"

NS_BEGIN(Engine)
class ENGINE_DLL CBoxCollider : public CCollider
{
public:
	typedef struct BoxColliderDesc : public COLLIDER_DESC
	{
		_float3			vSize;
	}BOX_COLLIDER_DESC;

private:
	CBoxCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoxCollider(const CBoxCollider& rhs);
	virtual ~CBoxCollider() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					UpdateColiision(_matrix WorldMatrix) override;
	virtual _bool					Intersect(COLLIDER eType, CCollider* pTarget) override;
	virtual _bool					RayIntersect(COLLIDER eType, CCollider* pTarget, DEFAULT_HIT_DESC& OutDesc) override;
	virtual _bool					RayHit(_vector vOrizin, _vector vDiraction, DEFAULT_HIT_DESC& OutDesc) override;
	virtual	ContainmentType			Contains(_vector vPoint) override;

	void							SetCollision(_float3 vCenter, _float3 vExtents);

#ifdef _DEBUG
	virtual HRESULT					Render() override;
#endif // _DBUG

	const 	BoundingBox&			GetBounding() { return *m_Bounding; }

private:
	BoundingBox*					m_OriginBounding = nullptr;
	BoundingBox*					m_Bounding = nullptr;

public:
	static		CBoxCollider*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CComponent*			Clone(void* pArg) override;
	virtual		void				Free()  override;
};
NS_END