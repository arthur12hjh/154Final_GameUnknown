#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Client)
class CMonsterDeadState final : public CState
{
private:
	CMonsterDeadState();
	virtual ~CMonsterDeadState() = default;

public:
	//생성 할때 Create 함수에 집어넣어 주세요
	virtual		HRESULT							Initialize(void* pArg);

	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual		void							Start(void* pArg = nullptr);

	virtual		void							Update(_float fTimeDelta);
	virtual		void							End();

public:
	static	CMonsterDeadState*					Create(void* pArg);
	virtual	void								Free() override;
};
NS_END