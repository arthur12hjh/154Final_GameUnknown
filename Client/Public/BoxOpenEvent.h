#pragma once
#include "Client_Defines.h"
#include "EventHandle.h"

NS_BEGIN(Client)
class CBoxOpenEvent final : public CEventHandle
{
private:
	CBoxOpenEvent(function<void(void*)> Func);
	virtual ~CBoxOpenEvent() = default;

public:
	static		CBoxOpenEvent*	Create(function<void(void*)> Func);
	virtual		void			Free() override;

};
NS_END