#include "OBBCollider.h"
#include "BoxCollider.h"
#include "SphereCollider.h"

#include "Transform.h"

#include "DebugDraw.h"
#include "GameObject.h"

COBBCollider::COBBCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CCollider(pDevice, pContext)
{
}

COBBCollider::COBBCollider(const COBBCollider& rhs) :
    CCollider(rhs)
{
}

HRESULT COBBCollider::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    m_eType = COLLIDER::OBB;
    return S_OK;
}

HRESULT COBBCollider::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    OBB_COLLIDER_DESC* Desc = static_cast<OBB_COLLIDER_DESC*>(pArg);

    _float4		vQuaternion = {};
    XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(Desc->vAngles.x, Desc->vAngles.y, Desc->vAngles.z));

    m_OriginOrientBox = new BoundingOrientedBox(Desc->vCenter, Desc->vSize, vQuaternion);
    m_Bounding = new BoundingOrientedBox(*m_OriginOrientBox);

    return S_OK;
}

void COBBCollider::UpdateColiision(_matrix WorldMatrix)
{
    m_bIsHit = false;
    m_OriginOrientBox->Transform(*m_Bounding, WorldMatrix);

    XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

_bool COBBCollider::Intersect(COLLIDER eType, CCollider* pTarget)
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

_bool COBBCollider::RayIntersect(COLLIDER eType, CCollider* pTarget, DEFAULT_HIT_DESC& OutDesc)
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
        vDireaction = XMVector3Normalize(TargetPosition - vCenter);
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
        vDireaction = XMVector3Normalize(TargetPosition - vCenter);
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
        vDireaction = XMVector3Normalize(TargetPosition - vCenter);
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

_bool COBBCollider::RayHit(_vector vOrizin, _vector vDiraction, DEFAULT_HIT_DESC& OutDesc)
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

_bool COBBCollider::FrustomIntersect(const BoundingFrustum& Frustom)
{
    return m_Bounding->Intersects(Frustom);
}

ContainmentType COBBCollider::Contains(_vector vPoint)
{
    return m_Bounding->Contains(vPoint);
}

void COBBCollider::SetCollision(_float3 vCenter, _float4 vAngle, _float3 vExtents)
{
    m_OriginOrientBox->Center = vCenter;
    m_OriginOrientBox->Extents = vExtents;

    _float4		vQuaternion = {};
    XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(vAngle.x, vAngle.y, vAngle.z));
    m_OriginOrientBox->Orientation = vQuaternion;
}

#ifdef _DEBUG
HRESULT COBBCollider::Render()
{
    __super::Render();
    m_pBatch->Begin();

    DX::Draw(m_pBatch, *m_Bounding, false == m_bIsHit ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));

    m_pBatch->End();
    return S_OK;
}

HRESULT COBBCollider::Render(_float4 vColor)
{
    __super::Render(vColor);
    m_pBatch->Begin();

    DX::Draw(m_pBatch, *m_Bounding, XMLoadFloat4(&vColor));

    m_pBatch->End();
    return S_OK;
}
#endif // _DEBUG

COBBCollider* COBBCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    COBBCollider* pOBBColiision = new COBBCollider(pDevice, pContext);
    if (FAILED(pOBBColiision->Initialize_Prototype()))
    {
        Safe_Release(pOBBColiision);
        MSG_BOX("CREATE FAIL : OBB COLLISION");
    }
    return pOBBColiision;
}

CComponent* COBBCollider::Clone(void* pArg)
{
    COBBCollider* pOBBColiision = new COBBCollider(*this);
    if (FAILED(pOBBColiision->Initialize(pArg)))
    {
        Safe_Release(pOBBColiision);
        MSG_BOX("CLONE FAIL : OBB COLLISION");
    }
    return pOBBColiision;
}

void COBBCollider::Free()
{
    __super::Free();
    Safe_Delete(m_OriginOrientBox);
    Safe_Delete(m_Bounding);
}
