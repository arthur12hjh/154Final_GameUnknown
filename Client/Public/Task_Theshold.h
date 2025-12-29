#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CNaytiba;
class CBossBlackBoard;

class CTask_Theshold : public CTask
{
protected:
	CTask_Theshold();
	virtual ~CTask_Theshold() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

private :
	CNaytiba*							m_pOwner = { nullptr };
	CBossBlackBoard*					m_pBlackBoard = { nullptr };

	_uint								m_iNumSelection = { };

public:
	static	CTask_Theshold*					Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;
	
};
NS_END