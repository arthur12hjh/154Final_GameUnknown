#pragma once
#include "BehaviorNode.h"

NS_BEGIN(Engine)
class ENGINE_DLL CSquenceNode : public CBehaviorNode
{
private:
	CSquenceNode();
	virtual ~CSquenceNode() = default;

public:
	virtual	HRESULT						Initialize_Prototype(const CBehaviorTree* pOwnerTree);

	// 데코레이터 성공 여부 반환
	virtual	_bool						Update(_float fTimeDelta);

private:
	list<CBehaviorNode*>				m_Decorators;
	list<CBehaviorNode*>				m_Services;
	list<CBehaviorNode*>				m_Actions;

public:
	static	CSquenceNode*				Create(const CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;
};
NS_END