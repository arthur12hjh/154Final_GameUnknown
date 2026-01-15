#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CNaytiba;
class CBossBlackBoard;

class CTask_Groggy : public CTask
{
	enum class GROGGY_STATE { START, LOOP, END };

protected:
	CTask_Groggy();
	virtual ~CTask_Groggy() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

private:
	CNaytiba*							m_pOwner = { nullptr };
	CBossBlackBoard*					m_pBlackBoard = { nullptr };

	string								m_szAnimation = {};


	_bool								m_bIsGorillaSound[2];
	GROGGY_STATE						m_eState = {};

private :
	void								ChangeGroggyState(GROGGY_STATE eState);
	void								Play_Start_GorggySound();
	void								Play_Loop_GorggySound();

	void								Play_EndSound();
	void								Gorilla_GrooggyEnd();
	void								Scarelt_GrooggyEnd();

public:
	static	CTask_Groggy*				Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;
};
NS_END