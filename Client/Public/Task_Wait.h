#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CTask_Wait final : public CTask
{
private:
	CTask_Wait();
	virtual ~CTask_Wait() = default;

public:
	HRESULT								Initialize_Prototype(const CBehaviorTree* pOwnerTree, _float fWaitTime);

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

private :
	_float2								m_vTime = {};

public:
	static	CTask_Wait*					Create(const CBehaviorTree* pOwnerTree, _float fWaitTime);
	virtual	void						Free() override;
};
NS_END