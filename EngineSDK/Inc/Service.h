#pragma once
#include "BehaviorNode.h"

NS_BEGIN(Engine)
class ENGINE_DLL CService abstract : public CBehaviorNode
{
protected :
	CService();
	virtual ~CService() = default;

public:
	virtual		HRESULT					Initialize_Prototype(const CBehaviorTree* pOwnerTree);
	virtual		NODE_STATE				Update(_float fTimeDelta);

	float								GetTickTime() { return m_fTickTime; }

protected :
	_float								m_fTickTime = {};

public:
	virtual	void						Free() override;
};
NS_END