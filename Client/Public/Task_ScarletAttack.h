#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CNayitba;
class CScarletBlackBoard;
class CGorillaBehaviorTree;
class CGameManager;

struct Character_Skill_Desc;

class CTask_ScarletAttack : public CTask
{
protected:
	CTask_ScarletAttack();
	virtual ~CTask_ScarletAttack() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

private:
	CNayitba*							m_pOwner = { nullptr };
	CScarletBlackBoard*					m_pBlackBoard = { nullptr };
	CGameManager*						m_pGameManager = { nullptr };
	queue<const Character_Skill_Desc*>	m_pSkillData = { };

	CGameObject*						m_pTarget = { nullptr };
	_int								m_PrePatternIndex = {};
	_int								m_CurPatternIndex = {};

	_float								m_fMaxDelayTime = {};
	_float								m_fMoveAnimMaxRatio = {};
	_float3								m_fAttackMovePoint = {};

	_float3								m_vDir = {};
	_float								m_fLerpSpeed = {};

	_float								m_fAnimationSpeed = {1.5f};
	_bool								m_bIsLookAtPoint = { false };
	_bool								m_bIsAttackStartLerp = {};

private:
	// 보스 패턴에 대한 정보
#pragma region Boss Pattern
	void								SelectAttackData();
	_bool								SelectPattern(_bool bIsRandom = true);

	void								NormalAttackPattern();
	void								LinkAttackPattern();

	void								EntranceAttack();

#pragma region Phase2
	void								SecondPhaseNormalAttack();

	void								SecondPhaseAttack();
#pragma endregion



#pragma endregion
	_bool								AttackActionAmount(_float fTimeDelta);
	_bool								Compute_AttackCoolTime(_bool bIsForce = false);

	void								AttackLerpMove(_float fTimeDelta);
	void								AttackADDMove(_float fTimeDelta, _float fSpeed);

	void								LookAtPoint(_float fTimeDelta);
	void								ResetAttackTask(_bool bIsCoolTime = true);

public:
	static	CTask_ScarletAttack*		Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;

};
NS_END