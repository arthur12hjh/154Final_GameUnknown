#include "CCTFilterCallback.h"

DefaultCCTFilterCallback::DefaultCCTFilterCallback()
{
}

DefaultCCTFilterCallback* DefaultCCTFilterCallback::Create()
{
    return new DefaultCCTFilterCallback();
}

void DefaultCCTFilterCallback::Free()
{
    __super::Free();
}
