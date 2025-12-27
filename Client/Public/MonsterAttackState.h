#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Client)
struct Character_Skill_Desc;
struct Naytiba_NetWork_Desc;

class CMonsterAttackState final : public CState
{
public :
	typedef struct MonsterAttackDesc
	{
		CGameObject*					pTarget;
		function<void(_float)>			AttackCompletedFunc;
	}MONSTER_ATTACK_DESC;

private:
	CMonsterAttackState();
	virtual ~CMonsterAttackState() = default;

public:
	//생성 할때 Create 함수에 집어넣어 주세요
	virtual		HRESULT							Initialize(void* pArg);

	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual		void							Start(void* pArg = nullptr, CState* pPreState = nullptr);

	virtual		void							Update(_float fTimeDelta);
	virtual		void							End();

	const Character_Skill_Desc*					GetSkillData() { return m_pSkillData; }

private :
	const Naytiba_NetWork_Desc*					m_StaticMonsterData = { nullptr };
	const Character_Skill_Desc*					m_pSkillData = { nullptr };

	CGameObject*								m_pTarget = { nullptr };

	// 특정 몬스터의 행동에 대한 정의
	_bool										m_bIsPattern = {};
	_bool										m_bIsMoveAction = {};

	_float3										m_vMoveDir = {};
	_float										m_fDistance = {};
	_float										m_fMoveAnimMaxRatio = {};

	_float										m_fMoveSpeed = {};
	_float										m_fPlayRatio = { 1.f };

	function<void(_float)>						m_AttackCompletedFunc;

private :
	void										ReadySetting();

	void										BeholderPattern(_float fTimeDelta);
	void										BanaclePattern(_float fTimeDelta);
	void										StatueAPattern(_float fTimeDelta);
	void										StatueBPattern(_float fTimeDelta);
	void										SunFlowerPattern(_float fTimeDelta);
	void										Minion11Pattern(_float fTimeDelta);
	void										TentaclePattern(_float fTimeDelta);

	void										SearchTargetDistance();
	void										LerpMoveAction(_float fTimeDelta, _float fSpeed);
	_vector										LerpRotation(_float fRatio, _float fSpeed = 1.f);


public:
	static	CMonsterAttackState*				Create(void* pArg);
	virtual	void								Free() override;
};
NS_END