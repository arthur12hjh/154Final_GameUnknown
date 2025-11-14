#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CBehaviorTree;

class ENGINE_DLL CBehaviorNode abstract : public CBase
{
public :
	enum class BEHAVIOR_NODE_TYPE { SELECTOR, SQUENCE, TASK, SERVICE, DECORATOR, END };
	enum class NODE_STATE { COMPLETE, RUNNING, FAIL, END };

protected :
	CBehaviorNode();
	virtual ~CBehaviorNode() = default;

public :
	virtual		HRESULT					Initialize_Prototype(const CBehaviorTree* pOwnerTree);

	virtual		NODE_STATE				Update(_float fTimeDelta);
	
	const BEHAVIOR_NODE_TYPE&			GetNodeType() { return m_eNodeType; }

protected :
	const	CBehaviorTree*				m_pOwnerTree = nullptr;
	BEHAVIOR_NODE_TYPE					m_eNodeType = { BEHAVIOR_NODE_TYPE::END };

public :
	virtual	void						Free();

};
NS_END