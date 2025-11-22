#include "pch.h"
#include "RewordEvent.h"

CRewordEvent::CRewordEvent(function<void(void*)> Func) :
    CEventHandle(Func)
{
}


CRewordEvent* CRewordEvent::Create(function<void(void*)> Func)
{
    return new CRewordEvent(Func);
}

void CRewordEvent::Free()
{
    __super::Free();
}
