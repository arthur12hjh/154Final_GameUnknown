#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CTask_Move : public CTask
{
protected:
	CTask_Move();
	virtual ~CTask_Move() = default;

public:
	virtual	HRESULT						Initialize_Prototype(const CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

public:
	static	CTask_Move*					Create(const CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;
};
NS_END