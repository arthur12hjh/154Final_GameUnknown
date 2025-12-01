#pragma once
#include "BehaviorNode.h"

NS_BEGIN(Engine)
class CBehaviorTree;

class ENGINE_DLL CDecorator abstract : public CBehaviorNode
{
protected:
	CDecorator();
	virtual ~CDecorator() = default;

public:
	virtual		HRESULT					Initialize_Prototype(CBehaviorTree* pOwnerTree);

	// 데코레이터 성공 여부 반환
	virtual	NODE_STATE					Update(_float fTimeDelta);

public:
	virtual	void						Free() override;
};
NS_END