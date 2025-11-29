#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
struct Character_Skill_Desc;

class CMonsterSuccessActionState final : public CState
{
public:
	typedef struct MonsterSucessStateDesc
	{
			
	}MONSTER_SUCESS_STATE_DESC;

private:
	CMonsterSuccessActionState();
	virtual ~CMonsterSuccessActionState() = default;

public:
	//생성 할때 Create 함수에 집어넣어 주세요
	virtual		HRESULT							Initialize(void* pArg);

	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual		void							Start(void* pArg = nullptr, CState* pPreState = nullptr) override;

	virtual		void							Update(_float fTimeDelta) override;
	virtual		void							End() override;

private :
	const Character_Skill_Desc*					m_pSkillData = { nullptr };

public:
	static	CMonsterSuccessActionState*			Create(void* pArg);
	virtual	void								Free() override;
};
NS_END