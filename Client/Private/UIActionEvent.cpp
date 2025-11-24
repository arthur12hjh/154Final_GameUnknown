#include "pch.h"
#include "UIActionEvent.h"

CUIActionEvent::CUIActionEvent(function<void(void*)> Func) :
    CEventHandle(Func)
{
}

CUIActionEvent* CUIActionEvent::Create(function<void(void*)> Func)
{
    int a = 0;

    return new CUIActionEvent(Func);
}

void CUIActionEvent::Free()
{
    __super::Free();
}
