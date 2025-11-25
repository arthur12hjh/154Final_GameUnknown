#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
struct Naytiba_Desc;
class CMonsterTranslationState final : public CState
{
public :
	typedef struct MonsterTranslationStateDesc
	{
		const char*				szTranslationAnimName;
	}MONSTER_TRANSLATION_STATE;

private:
	CMonsterTranslationState();
	virtual ~CMonsterTranslationState() = default;

public:
	//생성 할때 Create 함수에 집어넣어 주세요
	virtual		HRESULT							Initialize(void* pArg);

	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual		void							Start(void* pArg = nullptr, CState* pPreState = nullptr) override;

	virtual		void							Update(_float fTimeDelta) override;
	virtual		void							End() override;

public:
	static	CMonsterTranslationState*			Create(void* pArg);
	virtual	void								Free() override;
};
NS_END