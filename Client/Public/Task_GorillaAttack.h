#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CNaytiba;
class CBossBlackBoard;
class CGorillaBehaviorTree;
class CGameManager;

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
	CNaytiba*							m_pOwner = { nullptr };
	CBossBlackBoard*					m_pBlackBoard = { nullptr };
	CGameManager*						m_pGameManager = { nullptr };
	queue<const Character_Skill_Desc*>	m_pSkillData = { };

	_int								m_PrePatternIndex = {};
	_int								m_CurPatternIndex = {};

	_float								m_fMaxDelayTime = {};
	_float								m_fMoveAnimMaxRatio = {};
	_float3								m_fAttackMovePoint = {};

	_float								m_fDir = {};
	_float								m_fLerpSpeed = {};

	_bool								m_bIsLookAtPoint = { false };
	_bool								m_bIsAttackStartLerp = {};
	
private :
	_bool								SelectPattern();

	// 보스 패턴에 대한 정보
#pragma region Boss Pattern
	void								SelectAttack(_bool bIsForce = false);
	void								CrushPattern();
	void								MoveAttackPattern();
	void								BackStepPattern();
	//void								BboyStepPattern();

	void								SelectRandomPattern(_bool bIsBeta = false);
#pragma endregion

	_bool								AttackMoveAction(_float fTimeDelta);
	_bool								Compute_AttackCoolTime(_bool bIsForce = false);
	
	void								AttackLerpMove(_float fTimeDelta);
	void								AttackADDMove(_float fTimeDelta);

	void								LookAtPoint(_float fTimeDelta);
	void								ResetAttackTask(_bool bIsCoolTime = true);

public:
	static	CTask_GorillaAttack*		Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;

};
NS_END