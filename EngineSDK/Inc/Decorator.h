#pragma once
#include "BehaviorNode.h"

NS_BEGIN(Engine)
class CBehaviorTree;

class ENGINE_DLL CDecorator abstract : public CBehaviorNode
{
private:
	CDecorator();
	virtual ~CDecorator() = default;

public:
	virtual	HRESULT						Initialize_Prototype();
	virtual	HRESULT						Initialize(void* pArg);

	// 데코레이터 성공 여부 반환
	virtual	_bool						Update(_float fTimeDelta);

public:
	virtual	void						Free() override;
};
NS_END