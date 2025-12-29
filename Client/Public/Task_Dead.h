#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CNaytiba;
class CBossBlackBoard;

class CTask_Dead : public CTask
{
protected:
	CTask_Dead();
	virtual ~CTask_Dead() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

private:
	CNaytiba*							m_pOwner = { nullptr };
	CBossBlackBoard*					m_pBlackBoard = { nullptr };
	_float								m_fDeadEndTime = {};
	_bool								m_bIsDeadEffect = { false };

public:
	static	CTask_Dead*					Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;

};
NS_END