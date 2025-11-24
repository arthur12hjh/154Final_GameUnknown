#pragma once
#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
struct Naytiba_Desc;

class CMonsterMoveState final : public CState
{
public:
	typedef struct MoveStateDesc
	{
		CGameObject* pTarget;
		const vector<_float3>*			PathFindingPoints;
		function<void(_float)>			OnMoveCompleted;
	}MOVE_STATE_DESC;

private:
	CMonsterMoveState();
	virtual ~CMonsterMoveState() = default;

public:
	//생성 할때 Create 함수에 집어넣어 주세요
	virtual		HRESULT							Initialize(void* pArg);

	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual		void							Start(void* pArg = nullptr);

	virtual		void							Update(_float fTimeDelta);
	virtual		void							End();

private:
	const Naytiba_Desc*							m_pOwnerInfo = { nullptr };
	CGameObject*								m_pTarget = { nullptr };

	DIRECTION									m_vMoveDirection = { DIRECTION::END };
	_float2										m_vMoveTime = {};
	_float3										m_vMoveDir = {};

	_bool										m_bIsCaution = { false };
	_float3										m_vMovePoint = {};
	
	// 목표 지점에 들어갔을떄
	function<void(_float)>						m_OnMoveCompleted;
private :
	void										Compute_MovePointDirection();

	void										Update_Caution(_float fTimeDelta);
	void										Update_Move(_float fTimeDelta);

public:
	static	CMonsterMoveState*					Create(void* pArg);
	virtual	void								Free() override;
};
NS_END