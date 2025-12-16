#pragma once

#include "Client_Defines.h"
#include "GameStruct.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
NS_END

NS_BEGIN(Client)

/* 이름은 락온인데 일단은 플레이어 FSM 3개 바꿔주는 녀석임.. */

class CCinematicManager : public CBase
{
private:
	CCinematicManager();
	virtual ~CCinematicManager() = default;

public:

public:
	HRESULT						Initialize();
	
	void						Start_Cinematic();
private:
	CGameInstance* m_pGameInstance = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };
	PLAYER_DESC* m_pPlayerDesc = { nullptr };

public:
	static CCinematicManager* Create();
	virtual void Free() override;
};

NS_END