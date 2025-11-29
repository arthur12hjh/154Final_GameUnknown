#pragma once

#include "Client_Defines.h"
#include "GameStruct.h"
#include "State.h"

NS_BEGIN(Client)

typedef struct tagPlayerState { 
	PLAYER_STATE eNextState = { PLAYER_STATE::STATE_END };
	PLAYER_MODE eMode = { PLAYER_MODE::END };
	_bool isChangeMode = { false };
	void* pArg = { nullptr };
} PLAYER_TRANSITION_DESC; 

typedef struct tagWalkStateDesc {
	_bool isLand = { false };
	_bool isEvade = { false };
} PLAYER_BATTLEWALK_DESC;

class CPlayerState abstract : public CBase
{
protected:
	CPlayerState();
	virtual ~CPlayerState() = default;

public:
	PLAYER_STATE Get_State() { return m_eState; }

public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void					Start(void* pArg = nullptr) = 0;
	virtual PLAYER_TRANSITION_DESC	Update(_float fTimeDelta) = 0;
	virtual void					End() = 0;

protected:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CGameManager* m_pGameManager = { nullptr };

	class CPlayer* m_pPlayer = { nullptr };
	//만약 STATE_END가 아니라면 상태 변경.
	PLAYER_TRANSITION_DESC   m_tNextState = {};
	PLAYER_STATE   m_eState = { PLAYER_STATE::STATE_END };
	const struct Player_Desc* m_Desc = { nullptr };

public:
	virtual	void					Free() override;

};

NS_END
