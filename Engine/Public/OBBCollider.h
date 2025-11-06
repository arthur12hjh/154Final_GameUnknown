#pragma once
#include "Collider.h"

NS_BEGIN(Engine)
class ENGINE_DLL COBBCollider : public CCollider
{
public:
	typedef struct OBBColliderDesc : public COLLIDER_DESC
	{
		_float3			vSize;
		_float3			vAngles;
	}OBB_COLLIDER_DESC;

private:
	COBBCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COBBCollider(const COBBCollider& rhs);
	virtual ~COBBCollider() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					UpdateColiision(_matrix WorldMatrix) override;
	virtual _bool					Intersect(COLLIDER eType, CCollider* pTarget) override;
	virtual _bool					RayIntersect(COLLIDER eType, CCollider* pTarget, DEFAULT_HIT_DESC& OutDesc) override;
	virtual _bool					RayHit(_vector vOrizin, _vector vDiraction, DEFAULT_HIT_DESC& OutDesc) override;
	virtual	ContainmentType			Contains(_vector vPoint) override;

	void							SetCollision(_float3 vCenter, _float4 vAngle, _float3 vExtents);

#ifdef _DEBUG
	virtual HRESULT					Render() override;
#endif // _DBUG

	const 	BoundingOrientedBox&	GetBounding() { return *m_Bounding; }

private:
	BoundingOrientedBox*					m_OriginOrientBox = nullptr;
	BoundingOrientedBox*					m_Bounding = nullptr;

public:
	static		COBBCollider*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CComponent*				Clone(void* pArg) override;
	virtual		void					Free()  override;

};
NS_END