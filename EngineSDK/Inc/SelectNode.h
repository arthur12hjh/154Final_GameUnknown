#pragma once
#include "BehaviorNode.h"

NS_BEGIN(Engine)
class ENGINE_DLL CSelectNode final : public CBehaviorNode
{
private:
	CSelectNode();
	virtual ~CSelectNode() = default;

public:
	virtual	HRESULT						Initialize_Prototype(const CBehaviorTree* pOwnerTree);

	// 데코레이터 성공 여부 반환
	virtual	_bool						Update(_float fTimeDelta);

private :
	list<CBehaviorNode*>				m_Decorators;
	list<CBehaviorNode*>				m_Services;
	CBehaviorNode*						m_Action[2] = { nullptr, nullptr };

public:
	static	CSelectNode*				Create(const CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;

};
NS_END