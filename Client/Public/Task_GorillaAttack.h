#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CNayitba;
class CBossBlackBoard;
class CGorillaBehaviorTree;

struct Character_Skill_Desc;

class CTask_GorillaAttack : public CTask
{
protected:
	CTask_GorillaAttack();
	virtual ~CTask_GorillaAttack() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

private :
	CNayitba*							m_pOwner = { nullptr };
	CBossBlackBoard*					m_pBlackBoard = { nullptr };
	const Character_Skill_Desc*			m_pSkillData = { nullptr };

	_uint2								m_iAttackCount = {};
	_float								m_fMaxDelayTime = {};
	string								m_szDebugAnimation = {};
	
private :
	_bool								SelectRandomPattern();
	_bool								AttackMoveAction();
	_bool								Compute_AttackCoolTime();
	

public:
	static	CTask_GorillaAttack*		Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;

};
NS_END