#pragma once
#include "BehaviorNode.h"

NS_BEGIN(Engine)
class ENGINE_DLL CSquenceNode final : public CBehaviorNode
{
private:
	CSquenceNode();
	virtual ~CSquenceNode() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree);

	// 데코레이터 성공 여부 반환
	virtual	CBehaviorNode::NODE_STATE	Update(_float fTimeDelta);
	void								Bind_BehaviorNode(CBehaviorNode* pNode);

private:
	vector<CBehaviorNode*>				m_Decorators;
	vector<CBehaviorNode*>				m_Services;
	vector<CBehaviorNode*>				m_Actions;

	_uint								m_iIndex = {};

public:
	static	CSquenceNode*				Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;
};
NS_END