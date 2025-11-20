#pragma once
#include "Client_Defines.h"
#include "EventHandle.h"

NS_BEGIN(Client)
class CRewordEvent final : public CEventHandle
{
private:
	CRewordEvent(function<void(void*)> Func);
	virtual ~CRewordEvent() = default;

public:
	static		CRewordEvent*	Create(function<void(void*)> Func);
	virtual		void			Free() override;
};
NS_END