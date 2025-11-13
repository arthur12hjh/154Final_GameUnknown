#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CBehaviorTree;

class ENGINE_DLL CBehaviorNode abstract : public CBase
{
public :
	enum class BEHAVIOR_NODE_TYPE { TASK, SERVICE, DECORATOR, END };

protected :
	CBehaviorNode();
	virtual ~CBehaviorNode() = default;

public :
	virtual		HRESULT					Initialize_Prototype(const CBehaviorTree* pOwnerTree);
	virtual		_bool					Update(_float fTimeDelta);

protected :
	const	CBehaviorTree*				m_pOwnerTree = nullptr;
	BEHAVIOR_NODE_TYPE					m_eNodeType = { BEHAVIOR_NODE_TYPE::END };

public :
	virtual	CBehaviorNode*				Clone(void* pArg);
	virtual	void						Free();

};
NS_END