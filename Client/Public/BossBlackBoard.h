#pragma once
#include "Client_Defines.h"
#include "BlackBoard.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
struct Naytiba_NetWork_Desc;
struct Naytiba_Desc;

class CBossBlackBoard abstract : public CBlackBoard
{
public:
	typedef struct BossBlackBoardDesc
	{
		CGameObject*					pOwner;

	}BOSS_BLACKBOARD_DESC;

protected :
	CBossBlackBoard();
	virtual ~CBossBlackBoard() = default;

public:
	virtual HRESULT						Initialize(void* pArg) override;

	// 보스 타겟 세팅
	void								SetTarget(CGameObject* pGameObject);
	CGameObject*						GetTarget() { return m_pTarget; }

	const Naytiba_NetWork_Desc*			GetBossDefaultInfo() { return m_BossDefualtInfo; }
	const Naytiba_Desc*					GetBossInfo() { return m_BossCurrentInfo; }

protected :
	CGameObject*						m_pTarget = { nullptr };

	const Naytiba_NetWork_Desc*			m_BossDefualtInfo = { nullptr };
	const Naytiba_Desc*					m_BossCurrentInfo = { nullptr };

public:
	virtual		void					Free() override;
};
NS_END