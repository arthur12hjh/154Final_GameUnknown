#pragma once
#include "Client_Defines.h"
#include "Decorator.h"

NS_BEGIN(Client)
class CBossBlackBoard;

class CDeco_CheckAlive final : public CDecorator
{
protected:
	CDeco_CheckAlive();
	virtual ~CDeco_CheckAlive() = default;

public:
	virtual		HRESULT					Initialize_Prototype(CBehaviorTree* pOwnerTree, _float fDeadPercent);
	virtual		NODE_STATE				Update(_float fTimeDelta) override;
	
private :
	CBossBlackBoard*					m_pBlackBoard = { nullptr };
	_float								m_fDeadPercent = { 0.f };

public:
	static	CDeco_CheckAlive*			Create(CBehaviorTree* pOwnerTree, _float fDeadPercent = 0.f);
	virtual	void						Free() override;

};
NS_END