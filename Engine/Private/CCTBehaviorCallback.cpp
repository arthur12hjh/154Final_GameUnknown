#include "CCTBehaviorCallback.h"

CCTBehaviorCallback::CCTBehaviorCallback()
{
}

PxControllerBehaviorFlags CCTBehaviorCallback::getBehaviorFlags(const PxShape& PxShape, const PxActor& PxActor)
{
	return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CCTBehaviorCallback::getBehaviorFlags(const PxController& controller)
{
	return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CCTBehaviorCallback::getBehaviorFlags(const PxObstacle& obstacle)
{
	return PxControllerBehaviorFlags();
}

void CCTBehaviorCallback::Free()
{
	__super::Free();

	if (nullptr != m_pController)
		m_pController = nullptr;
}
