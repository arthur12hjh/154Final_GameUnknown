#pragma once
#include "Client_Defines.h"
#include "Decorator.h"

NS_BEGIN(Client)
class CDeco_AttackRange : public CDecorator
{
protected:
	CDeco_AttackRange();
	virtual ~CDeco_AttackRange() = default;

public:
	virtual	HRESULT						Initialize_Prototype(const CBehaviorTree* pOwnerTree) override;

	// 데코레이터 성공 여부 반환
	virtual	NODE_STATE					Update(_float fTimeDelta);

public:
	CDeco_AttackRange*			Create(const CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;

};
NS_END