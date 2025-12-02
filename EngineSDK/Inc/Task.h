#pragma once
#include "BehaviorNode.h"

NS_BEGIN(Engine)
class ENGINE_DLL CTask abstract : public CBehaviorNode
{
protected:
	CTask();
	virtual ~CTask() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

public:
	virtual	void						Free() override;

};
NS_END