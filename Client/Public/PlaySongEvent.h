#pragma once
#include "Client_Defines.h"
#include "EventHandle.h"

NS_BEGIN(Client)
class CPlaySongEvent final : public CEventHandle
{
private:
	CPlaySongEvent(function<void(void*)> Func);
	virtual ~CPlaySongEvent() = default;

public:
	static		CPlaySongEvent*	Create(function<void(void*)> Func);
	virtual		void			Free() override;

};
NS_END