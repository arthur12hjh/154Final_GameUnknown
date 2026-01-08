#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CNaytiba;
class CScarletBlackBoard;
class CGameManager;

struct Character_Skill_Desc;

class CTask_ScarletAttack final : public CTask
{
public :
	typedef struct FinshedDelayDesc
	{
		_uint			iSkillID = {};

		_float2			vKeyFrame = {};
		_uint2			vStopFrame = {};
	}FINISHED_DELAY_DESC;


protected:
	CTask_ScarletAttack();
	virtual ~CTask_ScarletAttack() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

private:
	CNaytiba*							m_pOwner = { nullptr };
	CScarletBlackBoard*					m_pBlackBoard = { nullptr };
	CGameManager*						m_pGameManager = { nullptr };
	queue<const Character_Skill_Desc*>	m_pSkillData = { };
	queue<MOTION_TIME_DESC>				m_TimeLineDatas = {};

	CGameObject*						m_pTarget = { nullptr };
	_float								m_fMaxDelayTime = {};
	_float								m_fMoveAnimMaxRatio = {};
	_float3								m_fAttackMovePoint = {};

	_float3								m_vDir = {};
	_float								m_fLerpSpeed = {};

	_float								m_fAnimationSpeed = {1.5f};
	_bool								m_bIsLookAtPoint = { false };
	_bool								m_bIsAttackStartLerp = {};
	
	_bool								m_bIsAttackStopDelay = {};
	_bool								m_bIsAttackDelayCheck = {};
	FINISHED_DELAY_DESC					m_FinishedDelayDesc = { };

private:
	// 보스 패턴에 대한 정보
	void								ActionAmount(_float fTimeDelta);

#pragma region Boss Pattern
	void								SelectAttackData();
	_bool								SelectPattern(_bool bIsRandom = true);

	void								NormalAttackPattern();
	void								EntranceAttack();
	void								BackStepPattern();
	void								CancelSkillData();

#pragma region Phase2
	void								SecondPhaseNormalAttack();
	void								SecondPhaseAttack();
#pragma endregion

#pragma endregion

	void								SelectAttackMoveData(_uint iID);
	_bool								Compute_AttackCoolTime(_bool bIsForce = false);
	_bool								Finished_Animation(_float fTimeDelta);

	void								AttackLerpMove(_float fTimeDelta);

	void								LookAtPoint(_float fTimeDelta);
	void								ResetAttackTask(_bool bIsCoolTime = true);
	void								Clear_ScarletAttackTask();


public:
	static	CTask_ScarletAttack*		Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;

};
NS_END