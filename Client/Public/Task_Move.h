#pragma once
#include "Client_Defines.h"
#include "Task.h"

NS_BEGIN(Client)
class CNaytiba;
class CBossBlackBoard;

class CTask_Move : public CTask
{
protected:
	CTask_Move();
	virtual ~CTask_Move() = default;

public:
	virtual	HRESULT						Initialize_Prototype(CBehaviorTree* pOwnerTree) override;

	// 테스크의 성공유무반환
	virtual	NODE_STATE					Update(_float fTimeDelta) override;

private :
	CNaytiba*							m_pOwner = { nullptr };
	CBossBlackBoard*					m_pBlackBoard = { nullptr };

	DIRECTION							m_eDirection = {};

	_bool								m_bIsCaution = { false };
	_uint								m_iAnimSection = {};

	string								m_szAnimationName = {};

	_float3								m_vMoveDir = {};
	_float								m_fSpeed = {};

private :
	void								Refresh_MovePoint();

public:
	static	CTask_Move*					Create(CBehaviorTree* pOwnerTree);
	virtual	void						Free() override;
};
NS_END