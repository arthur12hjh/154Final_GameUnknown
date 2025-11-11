#include "pch.h"

#include "PlayerBehaviorCallback.h"

CPlayerBehaviorCallback::CPlayerBehaviorCallback()
{
}

PxControllerBehaviorFlags CPlayerBehaviorCallback::getBehaviorFlags(const PxShape& PxShape, const PxActor& PxActor)
{
    return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CPlayerBehaviorCallback::getBehaviorFlags(const PxController& controller)
{
    return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CPlayerBehaviorCallback::getBehaviorFlags(const PxObstacle& obstacle)
{
    return PxControllerBehaviorFlags();
}

CPlayerBehaviorCallback* CPlayerBehaviorCallback::Create()
{
    return new CPlayerBehaviorCallback();
}

void CPlayerBehaviorCallback::Free()
{
    __super::Free();
}
