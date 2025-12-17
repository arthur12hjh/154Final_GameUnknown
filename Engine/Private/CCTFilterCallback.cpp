#include "CCTFilterCallback.h"

CCTFilterCallback::CCTFilterCallback()
{
}

CCTFilterCallback*::CCTFilterCallback::Create()
{
    return new CCTFilterCallback();
}

void CCTFilterCallback::Free()
{
    __super::Free();
}
