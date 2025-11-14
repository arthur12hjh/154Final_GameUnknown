#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CTask_Idle : public CTask
{
protected:
	CTask_Idle();
	virtual ~CTask_Idle() = default;

public:
	virtual	HRESULT						Initialize_Prototype(const CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

public:
	static	CTask_Idle*					Create(const CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;
};
NS_END