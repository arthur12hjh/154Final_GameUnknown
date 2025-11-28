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

private :
	const Naytiba_NetWork_Desc*					m_StaticMonsterData = { nullptr };
	const Character_Skill_Desc*					m_pSkillData = { nullptr };

	CGameObject*								m_pTarget = { nullptr };
	_bool										m_bIsGara = {};
	_bool										m_bIsRootGara = { false };
	_float										m_fRootMotionRatio = {};

	_float3										m_vMoveDir = {};
	_float										m_fMoveSpeed = {};
	function<void(_float)>						m_AttackCompletedFunc;

private :
	void										GaraSetting();
	void										GaraHitBox();

	void										BeholderGara(_float fTimeDelta);
	void										StatueAGara(_float fTimeDelta);
	void										StatueBGara(_float fTimeDelta);


public:
	static	CMonsterAttackState*				Create(void* pArg);
	virtual	void								Free() override;
};
NS_END