#pragma once
#include "Client_Defines.h"
#include "Decorator.h"

NS_BEGIN(Client)
class CPhaseChange final : public CDecorator
{
protected:
	CPhaseChange();
	virtual ~CPhaseChange() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 데코레이터 성공 여부 반환
	virtual	NODE_STATE					Update(_float fTimeDelta);

public:
	CPhaseChange*						Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;

};
NS_END