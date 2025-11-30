#pragma once
#include "Client_Defines.h"
#include "Decorator.h"

NS_BEGIN(Client)
class CDeco_CheckAlive final : public CDecorator
{
protected:
	CDeco_CheckAlive();
	virtual ~CDeco_CheckAlive() = default;

public:
	virtual		HRESULT					Initialize_Prototype(const CBehaviorTree* pOwnerTree) override;
	virtual		NODE_STATE				Update(_float fTimeDelta) override;

public:
	static	CDeco_CheckAlive*			Create(const CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;

};
NS_END