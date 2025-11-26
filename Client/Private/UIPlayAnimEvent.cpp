#include "pch.h"
#include "UIPlayAnimEvent.h"

CUIPlayAnimEvent::CUIPlayAnimEvent(function<void(void*)> Func) :
    CEventHandle(Func)
{
}

CUIPlayAnimEvent* CUIPlayAnimEvent::Create(function<void(void*)> Func)
{
    return new CUIPlayAnimEvent(Func);
}

void CUIPlayAnimEvent::Free()
{
    __super::Free();
}
