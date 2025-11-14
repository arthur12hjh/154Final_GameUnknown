#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CTask_Dead : public CTask
{
protected:
	CTask_Dead();
	virtual ~CTask_Dead() = default;

public:
	virtual	HRESULT						Initialize_Prototype(const CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

public:
	static	CTask_Dead*					Create(const CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;
	
};
NS_END