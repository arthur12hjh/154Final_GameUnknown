#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CNayitba;
class CBossBlackBoard;

class CTask_ScarletMove : public CTask
{
protected:
	CTask_ScarletMove();
	virtual ~CTask_ScarletMove() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

private:
	CNayitba*							m_pOwner = { nullptr };
	CBossBlackBoard*					m_pBlackBoard = { nullptr };

	DIRECTION							m_eDirection = {};

	_bool								m_bIsCaution = { false };

	string								m_szAnimationName = {};
	_float3								m_vMoveDir = {};
	_float								m_fSpeed = {};

private:
	void								Refresh_MovePoint();

public:
	static	CTask_ScarletMove*			Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;
};
NS_END