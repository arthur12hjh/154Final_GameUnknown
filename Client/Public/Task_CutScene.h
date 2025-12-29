#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CScarletBlackBoard;
class CNaytiba;

class CTask_CutScene final : public CTask
{
protected:
	CTask_CutScene();
	virtual ~CTask_CutScene() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree);

	// 테스크 성공 여부 반환
	virtual	NODE_STATE					Update(_float fTimeDelta);

private:
	CScarletBlackBoard*					m_pBlackBoard = { nullptr };
	CNaytiba*							m_pNaytiba = { nullptr };

public:
	static	CTask_CutScene*				Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;
};
NS_END