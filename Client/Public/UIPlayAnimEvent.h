#pragma once
#include "Client_Defines.h"
#include "EventHandle.h"

NS_BEGIN(Client)
class CUIPlayAnimEvent final : public CEventHandle
{
private:
	CUIPlayAnimEvent(function<void(void*)> Func);
	virtual ~CUIPlayAnimEvent() = default;

public:
	static		CUIPlayAnimEvent*	Create(function<void(void*)> Func);
	virtual		void			Free() override;

};
NS_END