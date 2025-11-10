#include "pch.h"
#include "PlayerCCTHitReporter.h"

CPlayerCCTHitReporter::CPlayerCCTHitReporter()
    : CCTHitReporter {}
{
}

void CPlayerCCTHitReporter::onShapeHit(const PxControllerShapeHit& HitInfo)
{
    PxShape* shape = HitInfo.shape;
    if (!shape)
        return;

    PxActor* actor = HitInfo.actor;
    if (!actor)
        return;

    PxRigidDynamic* pDynamicActor = actor->is<PxRigidDynamic>();
    if (pDynamicActor)
    {
        // 밀기 방향 계산 (수직 성분 제외)
        PxExtendedVec3 vExPushDir = HitInfo.worldPos - m_pController->getPosition();
        PxVec3 vPushDir = PxVec3(static_cast<_float>(vExPushDir.x), static_cast<_float>(vExPushDir.y), static_cast<_float>(vExPushDir.z));

        // 영벡터가 아니라면
        if (vPushDir.magnitudeSquared() != 0.f)
        {
            vPushDir.normalize();
            _float fPushStrength = 15.0f; // 원하는 힘 크기

            pDynamicActor->wakeUp(); // 반드시 깨워야 CCT 밀림이 적용됨
            // Force 또는 Impulse 적용
            pDynamicActor->addForce(vPushDir * fPushStrength, PxForceMode::eIMPULSE);
        }
    }
}

void CPlayerCCTHitReporter::onControllerHit(const PxControllersHit& hit)
{
}

void CPlayerCCTHitReporter::onObstacleHit(const PxControllerObstacleHit& hit)
{
}

CPlayerCCTHitReporter* CPlayerCCTHitReporter::Create()
{
    return new CPlayerCCTHitReporter();
}

void CPlayerCCTHitReporter::Free()
{
    __super::Free();
}
