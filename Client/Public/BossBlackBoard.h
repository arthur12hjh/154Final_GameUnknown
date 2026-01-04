#pragma once
#include "Client_Defines.h"
#include "BlackBoard.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
struct Naytiba_NetWork_Desc;
struct Default_Damage_Desc;
struct Naytiba_Desc;

class CBossBlackBoard abstract : public CBlackBoard
{
public:
	enum class BOSS_PAHSE { FIRST, SECOND, THIRD, END };
	enum class BOSS_STATE
	{
		IDLE, ATTACK, INTERACTION_ATTACK, HIT, MOVE, GROGGY, THESHOLD, CUTSCENE, DEAD, END
	};

	typedef struct PhaseChangeDesc
	{
		_bool				bIsCutScene;
		_bool				bIsLastAttack;
	}PHASE_CHANGE_DESC;

	typedef struct BossBlackBoardDesc
	{
		CGameObject*					pOwner;

	}BOSS_BLACKBOARD_DESC;

protected :
	CBossBlackBoard();
	virtual ~CBossBlackBoard() = default;

public:
	virtual HRESULT						Initialize(void* pArg) override;
	virtual	_bool						EnterExcution(NAYITBA_EXECUTION_TYPE eExcution);
	virtual _bool						UnconditionallyAttack();

public :
	// 보스 타겟 세팅
	void								SetTarget(CGameObject* pGameObject);
	CGameObject*						GetTarget() { return m_pTarget; }

	const Naytiba_NetWork_Desc*			GetBossDefaultInfo() { return m_BossDefualtInfo; }
	const Naytiba_Desc*					GetBossInfo() { return m_BossCurrentInfo; }

	void								SetSuperAmor(_bool bFlag);
	_bool								IsSuperAmor() { return m_bIsSuperAmor; }

	void								SetTargetDistacne();
	_float								GetTargetDistance() { return m_fTargetDistance; }

	void								SetCurState(BOSS_STATE eState);

	void								Set_BossPhase(BOSS_PAHSE ePhase);
	void								Set_PlayCutScene();

	_bool								Is_PlayPhaseChangeCutScene();
	_uint								Get_NumBossPhases();
	_float								Get_CurrentPhaseLitmitPercent();

	const BOSS_PAHSE&					Get_BossPhase() { return m_eBossPhase; }
	_bool								IsLastPhase();

	const BOSS_STATE&					GetCurState() { return m_eCurState; }
	const BOSS_STATE&					GetPreState() const { return m_ePreState; }

	void								SetHitData(const Default_Damage_Desc* pSkill);
	const Default_Damage_Desc*			GetHitData();

	void								AccAttackDelay(_float fTimeDelta);
	void								ClearAttackTimer();

	void								SetAttackDelay(_float fDelay);
	_float								GetAttackDelay() { return m_fAttackDelay.y; }

	void								AccGroggyTime(_float fTimeDelta);
	void								EnterGroggy();
	
	_bool								ExitGroggy();

	void								SetPhaseLastAttack(_bool bIsFlag);
	_bool								IsCurrentPhaseLastAttackAction();
	_bool								IsPhaseLastAttack() { return m_bIsPhaseLastAttack; }


	_bool								bIsExcution();
	// 이걸로 공격 가능체크하고
	// 가능하면 True를 반환.
	_bool								IsAttackEnable();

	void								SetAttackData(const Character_Skill_Desc* pAttack_Data);
	const Character_Skill_Desc*			GetAttackData();

	void								SetParryAttack();
	void								ResetParryAttack();
	_bool								IsParryAttack() { return m_bIsParryAttack; }

	void								Reset_State();

protected :
	CGameObject*						m_pTarget = { nullptr };

	const Naytiba_NetWork_Desc*			m_BossDefualtInfo = { nullptr };
	const Naytiba_Desc*					m_BossCurrentInfo = { nullptr };
	
	// 스킬 구조체
	const Character_Skill_Desc*			m_pAttack_Skill = { nullptr };

	//데미지구조체
	Default_Damage_Desc					m_pHit_Data = { nullptr };

	_bool								m_bIsPhaseLastAttack = { false };
	NAYITBA_EXECUTION_TYPE						m_eExcution = { NAYITBA_EXECUTION_TYPE::END };
	_bool								m_bIsParryAttack = { false };

	_bool								m_bIsSuperAmor = { false };
	_float								m_fTargetDistance = {};
	_float2								m_fAttackDelay = {};
	_float2								m_vGroggyTime = { 0.f, 4.0f };

	BOSS_PAHSE									m_eBossPhase = { BOSS_PAHSE::FIRST };
	vector<pair<_float, PHASE_CHANGE_DESC>>		m_ChangePhaseRatio;

	BOSS_STATE							m_ePreState = { BOSS_STATE::END };
	BOSS_STATE							m_eCurState = { BOSS_STATE::END };

private :
	_bool								m_bIsHit = { false };

public:
	virtual		void					Free() override;
};
NS_END