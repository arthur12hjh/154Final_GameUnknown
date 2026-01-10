#pragma once
#include "Client_Defines.h"
#include "EventHandle.h"

NS_BEGIN(Client)
class CNpcEvent final : public CEventHandle
{
private:
	CNpcEvent(function<void(void*)> Func);
	virtual ~CNpcEvent() = default;

public:
	static		CNpcEvent*	Create(function<void(void*)> Func);
	virtual		void			Free() override;

};
NS_END