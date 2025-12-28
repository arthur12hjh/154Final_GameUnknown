#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CNaytiba;
class CBossBlackBoard;

class CTask_ScarletDead : public CTask
{
protected:
	CTask_ScarletDead();
	virtual ~CTask_ScarletDead() = default;

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
	static	CTask_ScarletDead*			Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;

};
NS_END
