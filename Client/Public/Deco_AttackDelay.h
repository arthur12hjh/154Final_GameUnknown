#pragma once
#include "Client_Defines.h"
#include "Decorator.h"

NS_BEGIN(Client)
class CBossBlackBoard;

class CDeco_AttackDelay : public CDecorator
{
protected:
	CDeco_AttackDelay();
	virtual ~CDeco_AttackDelay() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 데코레이터 성공 여부 반환
	virtual	NODE_STATE					Update(_float fTimeDelta);

private :
	CBossBlackBoard*					m_pBlackBoard = { nullptr };

	_float								m_fAttackDelay = {};

public:
	static	CDeco_AttackDelay*			Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;

};
NS_END