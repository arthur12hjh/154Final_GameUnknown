#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
struct Naytiba_Desc;

class CMonsterGroggyState final : public CState
{
private:
	CMonsterGroggyState();
	virtual ~CMonsterGroggyState() = default;

public:
	//생성 할때 Create 함수에 집어넣어 주세요
	virtual		HRESULT							Initialize(void* pArg);

	virtual		void							Start(void* pArg = nullptr, CState* pPreState = nullptr) override;
	virtual		void							Update(_float fTimeDelta) override;
	virtual		void							End() override;

private:
	_float2										m_vGroogyTime = {};

	_float3										m_vMoveDir = {};
	_float										m_vMoveSpeed = {};

public:
	static	CMonsterGroggyState*				Create(void* pArg);
	virtual	void								Free() override;

};
NS_END