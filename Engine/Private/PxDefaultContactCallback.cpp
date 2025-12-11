#include "PxDefaultContactCallback.h"

PxDefaultContactCallback::PxDefaultContactCallback()
{
}

PxDefaultContactCallback* PxDefaultContactCallback::Create()
{
	return new PxDefaultContactCallback();
}

void PxDefaultContactCallback::Free()
{
	__super::Free();
}
