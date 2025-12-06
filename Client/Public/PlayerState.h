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
 
typedef struct tagHitStateDesc {
	_float3 vImpactDir;
	_float  fImpact;
	_float3 vHitDir; 
	_float3 vHitPoint;
	_float4 vAttackerPos;
} PLAYER_HIT_DESC;

typedef struct tagLockonWalkStateDesc {
	_bool isAlreadyRunning = { false }; 
} PLAYER_LOCKON_WALK_DESC;

typedef struct tagParryStateDesc {
	_bool isImmediate = { false };
} PLAYER_PARRY_DESC;

class CPlayerState abstract : public CBase
{
protected:
	CPlayerState();
	virtual ~CPlayerState() = default;

public:
	PLAYER_STATE Get_State() { return m_eState; }
	virtual _bool isTransferAble(PLAYER_MODE ePlayerMode, PLAYER_STATE ePlayerState); 
public:
	// 이거 초기화할때 혹시나 초기값 바뀌는 경우 있을수 있으니
	// void* 디폴트 매개변수 잡아서 넘겨받습니다.
	virtual void					Start(void* pArg = nullptr, _float fBlendRatio = 0.12f) = 0;
	virtual PLAYER_TRANSITION_DESC	Update(_float fTimeDelta) = 0;

	// 애니마다 연결될때 자연스러워지는 BlendRatio가 다르므로, 
	// 상태 끝낼 때 End 함수에서 값 반환해서 
	// 다음 상태에서 받아 처리할 수 있도록 작성.
	virtual _float End() = 0;

protected:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CGameManager* m_pGameManager = { nullptr };

	class CPlayer* m_pPlayer = { nullptr };
	//만약 STATE_END가 아니라면 상태 변경.
	PLAYER_TRANSITION_DESC   m_tNextState = {};
	PLAYER_STATE   m_eState = { PLAYER_STATE::STATE_END };
	struct Player_Desc* m_Desc = { nullptr };
	_float m_fNextBlendRatio = { 0.12f };

public:
	virtual	void					Free() override;

};

NS_END
