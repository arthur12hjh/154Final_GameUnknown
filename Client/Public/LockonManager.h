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

class CLockonManager : public CBase
{
private:
	CLockonManager();
	virtual ~CLockonManager() = default;

public:
	void   Bind_Player(class CPlayer* pPlayer);
	_float Get_CurMinDist() { return m_fCurrentMinDist; }
	_bool  Get_Lockon() { return m_isLock; }

public:
	HRESULT		   Initialize();
	// 가장 가까운 타겟을 찾는 과정. 락온이 가능하다면 True를 반환한다.
	_bool		   Find_NearestTarget(_float fTimeDelta);
	// 락온 모드일때 타겟의 트랜스폼 가져오는 함수.
	// 락온 모드가 아니라면 nullptr을 반환한다.
	CTransform*					Get_TargetTransform();

	//이거 락온한 위치 받아올수있게 일단 반환함
	_vector						Get_LockOnPoint();

	// 실질적인 락온 로직이 들어가있음. 락온 모드일때 세팅해주는 함수.
	void						Lockon(_float fTimeDelta);
	void						Start_Lockon();
	void						Check_LinkAttack();
private:
	CGameInstance*				m_pGameInstance		= { nullptr };
	class CPlayer*				m_pPlayer			= { nullptr };
	PLAYER_DESC*				m_pPlayerDesc		= { nullptr };

	class CNaytiba*				m_pTarget			= { nullptr };
	_wstring					m_strMonsterLayerTag = { TEXT("Layer_Monster") };
	_float						m_fLockonTimer		= { 0.f };

	_float						m_fBattleToIdleTimeAcc  = { 0.f };
	_float						m_fCurrentMinDist		= { FLT_MAX };
	_bool						m_isLock				= { false };
	_bool						m_isBossLock			= { false };
	class CUIBase*				m_pLockonUI				= { nullptr };
	class CUIBase*				m_pMonsterVitalUI				= { nullptr }; // 테스트 용

public:
	static CLockonManager* Create();
	virtual void Free() override;
};

NS_END