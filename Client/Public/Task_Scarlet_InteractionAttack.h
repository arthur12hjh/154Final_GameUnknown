#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CNaytiba;
class CScarletBlackBoard;
class CGameManager;

struct Character_Skill_Desc;

class CTask_Scarlet_InteractionAttack final : public CTask
{
protected:
	CTask_Scarlet_InteractionAttack();
	virtual ~CTask_Scarlet_InteractionAttack() = default;

public:
	virtual	HRESULT								Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE							Update(_float fTimeDelta) override;

private :
	CNaytiba*									m_pOwner = { nullptr };
	CScarletBlackBoard*							m_pBlackBoard = { nullptr };
	CGameManager*								m_pGameManager = { nullptr };

	_float										m_fAnimationSpeed = { 2.f };

private :
	_bool										SelectAnimationData();

public:
	static	CTask_Scarlet_InteractionAttack*	Create(CBehaviorTree* pOwnerTree);
	virtual	void								Free() override;
};
NS_END