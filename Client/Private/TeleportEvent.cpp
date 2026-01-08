#include "pch.h"
#include "TeleportEvent.h"

CTeleportEvent::CTeleportEvent(function<void(void*)> Func) :
    CEventHandle(Func)
{
}

CTeleportEvent* CTeleportEvent::Create(function<void(void*)> Func)
{
    int a = 0;

    return new CTeleportEvent(Func);
}

void CTeleportEvent::Free()
{
    __super::Free();
}
