#include "pch.h"
#include "BoxOpenEvent.h"

CBoxOpenEvent::CBoxOpenEvent(function<void(void*)> Func) :
    CEventHandle(Func)
{
}

CBoxOpenEvent* CBoxOpenEvent::Create(function<void(void*)> Func)
{
    return new CBoxOpenEvent(Func);
}

void CBoxOpenEvent::Free()
{
    __super::Free();
}
