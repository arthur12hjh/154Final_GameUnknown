#pragma once

#include "Client_Defines.h"
#include "State.h"

NS_BEGIN(Client)

class CPlayerState abstract : public CBase
{
protected:
	CPlayerState();
	virtual ~CPlayerState() = default;

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void					Start(void* pArg = nullptr) = 0;
	virtual CPlayerState*			Update(_float fTimeDelta) = 0;
	virtual void					End() = 0;

protected:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CGameManager* m_pGameManager = { nullptr };

	class CPlayer* m_pPlayer = { nullptr };
	CPlayerState*  m_pNextState = { nullptr };
	
	const struct Player_Desc* m_Desc = { nullptr };
public:
	virtual	void					Free() override;

};

NS_END
