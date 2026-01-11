#include "pch.h"
#include "NpcEvent.h"

CNpcEvent::CNpcEvent(function<void(void*)> Func) :
    CEventHandle(Func)
{
}

CNpcEvent* CNpcEvent::Create(function<void(void*)> Func)
{
    int a = 0;

    return new CNpcEvent(Func);
}

void CNpcEvent::Free()
{
    __super::Free();
}
