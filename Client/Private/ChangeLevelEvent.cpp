#include "pch.h"
#include "ChangeLevelEvent.h"

CChangeLevelEvent::CChangeLevelEvent(function<void(void*)> Func) :
    CEventHandle(Func)
{
}

CChangeLevelEvent* CChangeLevelEvent::Create(function<void(void*)> Func)
{
    int a = 0;

    return new CChangeLevelEvent(Func);
}

void CChangeLevelEvent::Free()
{
    __super::Free();
}
