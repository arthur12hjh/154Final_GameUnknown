#include "BoxCollider.h"
#include "OBBCollider.h"
#include "SphereCollider.h"

#include "Transform.h"
#include "DebugDraw.h"
#include "GameObject.h"

CBoxCollider::CBoxCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CCollider(pDevice, pContext)
{
}

CBoxCollider::CBoxCollider(const CBoxCollider& rhs) :
    CCollider(rhs)
{
}

HRESULT CBoxCollider::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    m_eType = COLLIDER::AABB;
    return S_OK;
}

HRESULT CBoxCollider::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    BOX_COLLIDER_DESC* Desc = static_cast<BOX_COLLIDER_DESC*>(pArg);
    m_OriginBounding = new BoundingBox(Desc->vCenter, Desc->vSize);
    m_Bounding = new BoundingBox(*m_OriginBounding);

    return S_OK;
}

void CBoxCollider::UpdateColiision(_matrix WorldMatrix)
{
    m_bIsHit = false;
    WorldMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[0]);
    WorldMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[1]);
    WorldMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(WorldMatrix.r[2]);

    m_OriginBounding->Transform(*m_Bounding, WorldMatrix);
}

void CBoxCollider::SetCollision(_float3 vCenter, _float3 vExtents)
{
    m_OriginBounding->Center = vCenter;
    m_OriginBounding->Extents = vExtents;
}

_bool CBoxCollider::Intersect(COLLIDER eType, CCollider* pTarget)
{
    _bool bIsHit = false;

    if (HIT_TYPE::END == m_eOnlyHitType)
    {
        if (!IntersectAble(pTarget->GetCollierHitType()))
            return false;
    }
    else
    {
        if (m_eOnlyHitType != pTarget->GetCollierHitType())
            return false;
    }

    switch (eType)
    {
    case COLLIDER::AABB:
    {
        auto pBoundingBox = static_cast<CBoxCollider*>(pTarget)->GetBounding();
        bIsHit = m_Bounding->Intersects(pBoundingBox);
        RayIntersect(COLLIDER::AABB, pTarget, m_HitDesc);
    }
        break;
    case COLLIDER::SPHERE:
    {
        auto pBoundingBox = static_cast<CSphereCollider*>(pTarget)->GetBounding();
        bIsHit = m_Bounding->Intersects(pBoundingBox);
        RayIntersect(COLLIDER::SPHERE, pTarget, m_HitDesc);
    }
    break;
    case COLLIDER::OBB:
    {
        auto pBoundingBox = static_cast<COBBCollider*>(pTarget)->GetBounding();
        bIsHit = m_Bounding->Intersects(pBoundingBox);
        RayIntersect(COLLIDER::OBB, pTarget, m_HitDesc);
    }
    break;
    }

    return bIsHit;
}

_bool CBoxCollider::RayIntersect(COLLIDER eType, CCollider* pTarget, DEFAULT_HIT_DESC& OutDesc)
{
    _bool bIsHit = false;
    _vector TargetPosition{}, vDireaction{};
    _vector vCenter = XMLoadFloat3(&m_Bounding->Center);

    _float fDistance = { -1 };
    switch (eType)
    {
    case COLLIDER::AABB:
    {
        auto pTargetBoundBox = static_cast<CBoxCollider*>(pTarget)->GetBounding();
        TargetPosition = XMLoadFloat3(&pTargetBoundBox.Center);
        vDireaction = XMVector3Normalize(vCenter - TargetPosition);
        if (XMVector3Equal(vDireaction, XMVectorZero()))
            bIsHit = true;
        else
            bIsHit = m_Bounding->Intersects(TargetPosition, vDireaction, fDistance);
    }
    break;
    case COLLIDER::SPHERE:
    {
        auto pTargetBoundBox = static_cast<CSphereCollider*>(pTarget)->GetBounding();
        TargetPosition = XMLoadFloat3(&pTargetBoundBox.Center);
        vDireaction = XMVector3Normalize(vCenter - TargetPosition);
        if (XMVector3Equal(vDireaction, XMVectorZero()))
            bIsHit = true;
        else
            bIsHit = m_Bounding->Intersects(TargetPosition, vDireaction, fDistance);
    }
    break;
    case COLLIDER::OBB:
    {
        auto pTargetBoundBox = static_cast<COBBCollider*>(pTarget)->GetBounding();
        TargetPosition = XMLoadFloat3(&pTargetBoundBox.Center);
        vDireaction = XMVector3Normalize(vCenter - TargetPosition);
        if (XMVector3Equal(vDireaction, XMVectorZero()))
            bIsHit = true;
        else
            bIsHit = m_Bounding->Intersects(TargetPosition, vDireaction, fDistance);
    }
    break;
    }

    XMStoreFloat3(&OutDesc.vHitPoint, vCenter + vDireaction * fDistance);
    OutDesc.vfDistance = fDistance;
    XMStoreFloat3(&OutDesc.vDireaction, vDireaction);
    XMStoreFloat3(&OutDesc.vNormal, XMVector3Normalize(XMLoadFloat3(&OutDesc.vHitPoint) - TargetPosition));
    m_HitDesc = OutDesc;

    return bIsHit;
}

_bool CBoxCollider::RayHit(_vector vOrizin, _vector vDiraction, DEFAULT_HIT_DESC& OutDesc)
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

_bool CBoxCollider::FrustomIntersect(const BoundingFrustum& Frustom)
{
    return m_Bounding->Intersects(Frustom);
}

ContainmentType CBoxCollider::Contains(_vector vPoint)
{
    return m_Bounding->Contains(vPoint);
}

#ifdef _DEBUG
HRESULT CBoxCollider::Render()
{
    __super::Render();
    m_pBatch->Begin();

    DX::Draw(m_pBatch, *m_Bounding, false == m_bIsHit ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));

    m_pBatch->End();
    return S_OK;
}

HRESULT CBoxCollider::Render(_float4 vColor)
{
    __super::Render(vColor);
    m_pBatch->Begin();

    DX::Draw(m_pBatch, *m_Bounding, XMLoadFloat4(&vColor));

    m_pBatch->End();
    return S_OK;
}
#endif // _DEBUG

CBoxCollider* CBoxCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBoxCollider* pBoxCollision = new CBoxCollider(pDevice, pContext);
    if (FAILED(pBoxCollision->Initialize_Prototype()))
    {
        Safe_Release(pBoxCollision);
        MSG_BOX("CREATE FAIL : BOX COLLISION");
    }
    return pBoxCollision;
}

CComponent* CBoxCollider::Clone(void* pArg)
{
    CBoxCollider* pBoxCollision = new CBoxCollider(*this);
    if (FAILED(pBoxCollision->Initialize(pArg)))
    {
        Safe_Release(pBoxCollision);
        MSG_BOX("CLONE FAIL : BOX COLLISION");
    }
    return pBoxCollision;
}

void CBoxCollider::Free()
{
    __super::Free();

    Safe_Delete(m_OriginBounding);
    Safe_Delete(m_Bounding);
}