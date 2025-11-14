#pragma once
#include "Client_Defines.h"
#include "GameStruct.h"
#include "BlackBoard.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
class CBossBlackBoard abstract : public CBlackBoard
{
protected :
	CBossBlackBoard();
	virtual ~CBossBlackBoard() = default;

public:
	HRESULT								Initialize(_uint iBossID);

	// 보스 타겟 세팅
	void								SetTarget(CGameObject* pGameObject);
	CGameObject*						GetTarget() { return m_pTarget; }

	const BOSS_NETWORK_DESC*			GetBossDefaultInfo() { return m_BossDefualtInfo; }
	BOSS_DESC&							GetBossInfo() { return m_BossCurrentInfo; }

protected :
	CGameObject*						m_pTarget = { nullptr };

	const BOSS_NETWORK_DESC*			m_BossDefualtInfo = { nullptr };
	BOSS_DESC							m_BossCurrentInfo;

public:
	virtual		void					Free() override;
};
NS_END