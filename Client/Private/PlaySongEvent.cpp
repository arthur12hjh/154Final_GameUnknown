#include "pch.h"
#include "PlaySongEvent.h"

CPlaySongEvent::CPlaySongEvent(function<void(void*)> Func) :
    CEventHandle(Func)
{
}

CPlaySongEvent* CPlaySongEvent::Create(function<void(void*)> Func)
{
    int a = 0;

    return new CPlaySongEvent(Func);
}

void CPlaySongEvent::Free()
{
    __super::Free();
}
