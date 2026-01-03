#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CScarletBlackBoard;
class CNaytiba;

class CTask_CutScene final : public CTask
{
private :
	CTask_CutScene();
	virtual ~CTask_CutScene() = default;

public :
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree);
	virtual	NODE_STATE					Update(_float fTimeDelta);

private :
	CScarletBlackBoard*					m_pBlackBoard = { nullptr };
	CNaytiba*							m_pNaytiba = { nullptr };

	_bool								m_bIsPlayEnd = {};

private :
	void								FinishedCutScene();

public:
	static	CTask_CutScene*				Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;
};
NS_END