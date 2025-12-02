#pragma once
#include "Client_Defines.h"
#include "Decorator.h"

NS_BEGIN(Client)
class CBossBlackBoard;

class CDeco_FindTarget : public CDecorator
{
protected:
	CDeco_FindTarget();
	virtual ~CDeco_FindTarget() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 데코레이터 성공 여부 반환
	virtual	NODE_STATE					Update(_float fTimeDelta);

private:
	CBossBlackBoard*					m_pBlackBoard = { nullptr };

public:
	static	CDeco_FindTarget*			Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;
};
NS_END