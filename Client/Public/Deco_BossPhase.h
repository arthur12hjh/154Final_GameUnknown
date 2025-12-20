#pragma once
#include "Client_Defines.h"
#include "Decorator.h"

NS_BEGIN(Client)
class CBossBlackBoard;

class CDeco_BossPhase final : public CDecorator
{
protected:
	CDeco_BossPhase();
	virtual ~CDeco_BossPhase() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree);

	// 데코레이터 성공 여부 반환
	virtual	NODE_STATE					Update(_float fTimeDelta);

private :
	CBossBlackBoard*					m_pBlackBoard = { nullptr };
	long long							m_iBossMaxHealth = {};

public:
	static	CDeco_BossPhase*			Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;

};
NS_END