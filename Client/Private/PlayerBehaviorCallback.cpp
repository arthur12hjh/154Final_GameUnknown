#include "pch.h"

#include "PlayerBehaviorCallback.h"

CPlayerBehaviorCallback::CPlayerBehaviorCallback()
{
}

PxControllerBehaviorFlags CPlayerBehaviorCallback::getBehaviorFlags(const PxShape& PxShape, const PxActor& PxActor)
{
    if (PxActor.is<PxRigidDynamic>()) 
        return PxControllerBehaviorFlags();

    return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
}

PxControllerBehaviorFlags CPlayerBehaviorCallback::getBehaviorFlags(const PxController& controller)
{
    return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags CPlayerBehaviorCallback::getBehaviorFlags(const PxObstacle& obstacle)
{
    return PxControllerBehaviorFlags(0);
}

CPlayerBehaviorCallback* CPlayerBehaviorCallback::Create()
{
    return new CPlayerBehaviorCallback();
}

void CPlayerBehaviorCallback::Free()
{
    __super::Free();
}
