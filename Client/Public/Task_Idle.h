#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CNayitba;
class CBossBlackBoard;

class CTask_Idle : public CTask
{
protected:
	CTask_Idle();
	virtual ~CTask_Idle() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

private :
	CNayitba*							m_pOwner = { nullptr };
	CBossBlackBoard*					m_pBlackBoard = { nullptr };

	string								m_szAnimationName = {};

public:
	static	CTask_Idle*					Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;
};
NS_END