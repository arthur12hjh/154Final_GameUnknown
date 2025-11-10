#include "pch.h"
#include "PlayerCCTHitReporter.h"

CPlayerCCTHitReporter::CPlayerCCTHitReporter()
    : CCTHitReporter {}
{
}

/* 다른 RigidDynamic과 부딪혔을때. */
void CPlayerCCTHitReporter::onShapeHit(const PxControllerShapeHit& HitInfo)
{
    PxRigidDynamic* pDynamicActor = HitInfo.actor->is<PxRigidDynamic>();

    if (pDynamicActor)
    {
        PxUserData* pData = static_cast<PxUserData*>(HitInfo.actor->userData);

        if (pData->szActorTag != TEXT("Prop_Actor"))
            return;
        // 밀기 방향 계산 (수직 성분 제외)
        PxExtendedVec3 vExPushDir = HitInfo.worldPos - m_pController->getPosition();
        PxVec3 vPushDir = PxVec3(static_cast<_float>(vExPushDir.x), static_cast<_float>(vExPushDir.y), static_cast<_float>(vExPushDir.z));

        // 영벡터가 아니라면
        if (vPushDir.magnitudeSquared() != 0.f)
        {
            _wstring szActorTag = static_cast<PxUserData*>(pDynamicActor->userData)->szActorTag;
            vPushDir.normalize();
            _float fPushStrength = 15.0f; // 원하는 힘 크기

            pDynamicActor->wakeUp(); // 반드시 깨워야 CCT 밀림이 적용됨
            // Force 또는 Impulse 적용
            pDynamicActor->addForce(vPushDir * fPushStrength, PxForceMode::eIMPULSE);
        }
    }
}

/* 다른 컨트롤러와 부딪혔을 때. */
void CPlayerCCTHitReporter::onControllerHit(const PxControllersHit& hit)
{
}

/* 정확히 모름.. 공부좀 해볼게요. */
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
