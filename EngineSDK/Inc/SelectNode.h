#pragma once
#include "BehaviorNode.h"

NS_BEGIN(Engine)
class ENGINE_DLL CSelectNode final : public CBehaviorNode
{
private:
	CSelectNode();
	virtual ~CSelectNode() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree);

	// 데코레이터 성공 여부 반환
	virtual	NODE_STATE					Update(_float fTimeDelta);
	void								Bind_BehaviorNode(CBehaviorNode* pNode);

protected :
	vector<CBehaviorNode*>				m_Decorators;
	vector<CBehaviorNode*>				m_Services;
	vector<CBehaviorNode*>				m_Actions;

private :
	_uint								m_iIndex = {};

public:
	static	CSelectNode*				Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;

};
NS_END