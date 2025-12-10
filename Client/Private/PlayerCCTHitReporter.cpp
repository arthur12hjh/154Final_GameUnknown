#include "pch.h"
#include "PlayerCCTHitReporter.h"

#include "RigidBody.h"
#include "GameObject.h"

CPlayerCCTHitReporter::CPlayerCCTHitReporter()
    : CCTHitReporter {}
{
}

/* 다른 RigidDynamic과 부딪혔을때. */
void CPlayerCCTHitReporter::onShapeHit(const PxControllerShapeHit& HitInfo)
{
    PxRigidActor* pActor = HitInfo.actor;

    switch (pActor->getConcreteType())
    {
        // 다이나믹 + 키네마틱 분기
    case PxConcreteType::eRIGID_DYNAMIC:
    {
        PxRigidDynamic* pDynamic = static_cast<PxRigidDynamic*>(pActor);
        PxUserData* pData = static_cast<PxUserData*>(HitInfo.actor->userData);

        // 키네마틱
        if (pDynamic->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
        {
            if (pData->szActorTag == TEXT("Elevator_Platform"))
            {
                CGameObject* pActor = static_cast<CRigidBody*>(pData->pHitActor)->GetOwner();
                m_pCCT->Set_Riding(true, pActor->GetTransform(), static_cast<CRigidBody*>(pData->pHitActor));
                m_vHitPos = PxVec3(HitInfo.worldPos.x, HitInfo.worldPos.y, HitInfo.worldPos.z);
            }
        }
        // 다이나믹
        else
        {
            if (pData->szActorTag == TEXT("Prop_Actor"))
            {
                // 밀기 방향 계산 (수직 성분 제외)
                PxExtendedVec3 vExPushDir = HitInfo.worldPos - m_pController->getPosition();
                PxVec3 vPushDir = PxVec3(static_cast<_float>(vExPushDir.x), static_cast<_float>(vExPushDir.y), static_cast<_float>(vExPushDir.z));

                // 영벡터가 아니라면
                if (vPushDir.magnitudeSquared() != 0.f)
                {
                    _wstring szActorTag = static_cast<PxUserData*>(pDynamic->userData)->szActorTag;
                    vPushDir.normalize();
                    _float fPushStrength = 15.0f; // 원하는 힘 크기

                    // Force 또는 Impulse 적용
                    pDynamic->addForce(vPushDir * fPushStrength, PxForceMode::eIMPULSE);
                }
            }

            if (pData->szActorTag == TEXT("Non_Collidable"))
            {
                // 밀기 방향 계산 (수직 성분 제외)
                PxExtendedVec3 vExPushDir = HitInfo.worldPos - m_pController->getPosition();
                PxVec3 vPushDir = PxVec3(static_cast<_float>(vExPushDir.x), static_cast<_float>(vExPushDir.y), static_cast<_float>(vExPushDir.z));

                // 영벡터가 아니라면
                if (vPushDir.magnitudeSquared() != 0.f)
                {
                    _wstring szActorTag = static_cast<PxUserData*>(pDynamic->userData)->szActorTag;
                    vPushDir.normalize();
                    _float fPushStrength = 15.0f; // 원하는 힘 크기

                    // Force 또는 Impulse 적용
                    pDynamic->addForce(vPushDir * fPushStrength, PxForceMode::eIMPULSE);
                }
            }
        }
        break;
    }

    case PxConcreteType::eRIGID_STATIC:
    {
        PxRigidStatic* pStatic = static_cast<PxRigidStatic*>(pActor);
        break;
    }

    default:
        break;
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
