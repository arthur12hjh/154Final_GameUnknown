#pragma once
#include "Client_Defines.h"
#include "EventHandle.h"

NS_BEGIN(Client)
class CChangeLevelEvent final : public CEventHandle
{
private:
	CChangeLevelEvent(function<void(void*)> Func);
	virtual ~CChangeLevelEvent() = default;

public:
	static		CChangeLevelEvent*	Create(function<void(void*)> Func);
	virtual		void			Free() override;

};
NS_END