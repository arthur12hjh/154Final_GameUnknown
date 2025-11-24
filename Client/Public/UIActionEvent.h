#pragma once
#include "Client_Defines.h"
#include "EventHandle.h"

NS_BEGIN(Client)
class CUIActionEvent final : public CEventHandle
{
private:
	CUIActionEvent(function<void(void*)> Func);
	virtual ~CUIActionEvent() = default;

public:
	static		CUIActionEvent*	Create(function<void(void*)> Func);
	virtual		void			Free() override;

};
NS_END