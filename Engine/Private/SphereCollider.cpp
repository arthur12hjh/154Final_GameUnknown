#include "SphereCollider.h"
#include "BoxCollider.h"
#include "OBBCollider.h"

#include "Transform.h"
#include "DebugDraw.h"
#include "GameObject.h"

CSphereCollider::CSphereCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CCollider(pDevice, pContext)
{
}

CSphereCollider::CSphereCollider(const CSphereCollider& rhs) :
	CCollider(rhs)
{
}

HRESULT CSphereCollider::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	m_CollisionType = COLLIDER::SPHERE;
	return S_OK;
}

HRESULT CSphereCollider::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	SPHERE_COLLIDER_DESC* Desc = static_cast<SPHERE_COLLIDER_DESC*>(pArg);
	m_OriginBounding = new BoundingSphere(Desc->vCenter, Desc->fRadius);
	m_Bounding = new BoundingSphere(*m_OriginBounding);

	return S_OK;
}

void CSphereCollider::UpdateColiision(_matrix WorldMatrix)
{
	m_bIsHit = false;

	m_OriginBounding->Transform(*m_Bounding, WorldMatrix);
}

_bool CSphereCollider::Intersect(COLLIDER eType, CCollider* pTarget)
{
	_bool bIsHit = false;

	if (HIT_TYPE::END == m_eOnlyHitType)
	{
		if (!IntersectAble(pTarget->GetCollisionHitType()))
			return false;
	}
	else
	{
		if (m_eOnlyHitType != pTarget->GetCollisionHitType())
			return false;
	}

	_vector OwnerPosition = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector TargetPosition = pTarget->GetOwner()->GetTransform()->Get_State(STATE::POSITION);
	_vector vDireaction = XMVector3Normalize(TargetPosition - OwnerPosition);

	_vector vCenter = XMLoadFloat3(&m_Bounding->Center);
	_float fDistance = XMVectorGetX(XMVector3Length(TargetPosition - OwnerPosition));

	switch (eType)
	{
	case COLLIDER::AABB:
		bIsHit = m_Bounding->Intersects(static_cast<CBoxCollider*>(pTarget)->GetBounding());
		break;
	case COLLIDER::SPHERE:
		bIsHit = m_Bounding->Intersects(static_cast<CSphereCollider*>(pTarget)->GetBounding());
		break;
	case COLLIDER::OBB:
		bIsHit = m_Bounding->Intersects(static_cast<COBBCollider*>(pTarget)->GetBounding());
		break;
	}

	XMStoreFloat3(&m_HitDesc.vHitPoint, vCenter + vDireaction * fDistance);
	m_HitDesc.vfDistance = fDistance;
	XMStoreFloat3(&m_HitDesc.vDireaction, vDireaction);
	XMStoreFloat3(&m_HitDesc.vNormal, XMVector3Normalize(XMLoadFloat3(&m_HitDesc.vHitPoint) - TargetPosition));
	return bIsHit;
}

_bool CSphereCollider::RayIntersect(COLLIDER eType, CCollider* pTarget, DEFAULT_HIT_DESC& OutDesc)
{
	_bool bIsHit = false;

	_vector OwnerPosition = m_pOwner->GetTransform()->Get_State(STATE::POSITION);
	_vector TargetPosition = pTarget->GetOwner()->GetTransform()->Get_State(STATE::POSITION);
	_vector vDireaction = XMVector3Normalize(TargetPosition - OwnerPosition);

	_vector vCenter = XMLoadFloat3(&m_Bounding->Center);
	_float fDistance = { -1 };
	switch (eType)
	{
	case COLLIDER::AABB:
		bIsHit = m_Bounding->Intersects(vCenter, vDireaction, fDistance);
		break;
	case COLLIDER::SPHERE:
		bIsHit = m_Bounding->Intersects(vCenter, vDireaction, fDistance);
		break;
	case COLLIDER::OBB:
		bIsHit = m_Bounding->Intersects(vCenter, vDireaction, fDistance);
		break;
	}

	XMStoreFloat3(&OutDesc.vHitPoint, vCenter + vDireaction * fDistance);
	OutDesc.vfDistance = fDistance;
	XMStoreFloat3(&OutDesc.vDireaction, vDireaction);
	XMStoreFloat3(&OutDesc.vNormal, XMVector3Normalize(XMLoadFloat3(&OutDesc.vHitPoint) - TargetPosition));
	m_HitDesc = OutDesc;

	return bIsHit;
}

_bool CSphereCollider::RayHit(_vector vOrizin, _vector vDiraction, DEFAULT_HIT_DESC& OutDesc)
{
	_bool bIsHit = false;
	_float fDistance = {};
	bIsHit = m_Bounding->Intersects(vOrizin, vDiraction, fDistance);

	XMStoreFloat3(&OutDesc.vHitPoint, vOrizin + vDiraction * fDistance);
	OutDesc.vfDistance = fDistance;
	XMStoreFloat3(&OutDesc.vDireaction, vDiraction);
	XMStoreFloat3(&OutDesc.vNormal, XMVector3Normalize(XMLoadFloat3(&OutDesc.vHitPoint) - vOrizin));
	return bIsHit;
}

_bool CSphereCollider::FrustomIntersect(const BoundingFrustum& Frustom)
{
	return m_Bounding->Intersects(Frustom);
}

ContainmentType CSphereCollider::Contains(_vector vPoint)
{
	return m_Bounding->Contains(vPoint);
}

void CSphereCollider::SetCollision(_float3 vCenter, _float fRadius)
{
	m_OriginBounding->Center = vCenter;
	m_OriginBounding->Radius = fRadius;
}

#ifdef _DEBUG
HRESULT CSphereCollider::Render()
{
	__super::Render();
	m_pBatch->Begin();

	DX::Draw(m_pBatch, *m_Bounding, false == m_bIsHit ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));

	m_pBatch->End();
	return S_OK;
}

HRESULT CSphereCollider::Render(_float4 vColor)
{
	__super::Render(vColor);
	m_pBatch->Begin();

	DX::Draw(m_pBatch, *m_Bounding, false == m_bIsHit ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));

	m_pBatch->End();
	return S_OK;
}
#endif // _DEBUG

CSphereCollider* CSphereCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSphereCollider* pSphereCollision = new CSphereCollider(pDevice, pContext);
	if (FAILED(pSphereCollision->Initialize_Prototype()))
	{
		Safe_Release(pSphereCollision);
		MSG_BOX("CREATE FAIL : SPEHRE COLLISION");
	}
	return pSphereCollision;
}

CComponent* CSphereCollider::Clone(void* pArg)
{
	CSphereCollider* pSphereCollision = new CSphereCollider(*this);
	if (FAILED(pSphereCollision->Initialize(pArg)))
	{
		Safe_Release(pSphereCollision);
		MSG_BOX("CLONE FAIL : SPEHRE COLLISION");
	}
	return pSphereCollision;
}

void CSphereCollider::Free()
{
	__super::Free();

	Safe_Delete(m_OriginBounding);
	Safe_Delete(m_Bounding);
}
