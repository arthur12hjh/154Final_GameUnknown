#pragma once
#include "Character.h"
/*
스포너는 기본적으로 2D 그리드 형태에서 나온다고 보면됩니다.
평지에서 생성하거나 언덕에다 생성하는 경우 잘배치해서 사용해 주세요

시간형(Time)  :	일정 주기마다 계속 몬스터를 만들어낸다.
				이때 횟수형태와 데미지를 받아서 죽는 형태를 선택할 수 있다.

트리거(Trigger) :  트리거 형태의 박스의 경우에는 콜리전 박스에 부딪힐 경우에
				  콜리전 범위안에 List에 저장된 몬스터들을 전부 소환한다.
*/

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Engine)
class CSpawner final : public CCharacter
{
public :
	enum class SPAWNER_TYPE { TIME, TRIGGER, END };
	enum class TIME_SPANWER_TYPE { LOOP, DESTRUCTTIBLE, END };

	typedef struct Spawner_Desc : public GAMEOBJECT_DESC
	{
		SPAWNER_TYPE			eType;
		
		// 시간형태의 스포너 일경우 설정
		TIME_SPANWER_TYPE		eTimeSpanwerType;
		_float					fSpawnTime;
		_uint					fLoopCount;
	}SPAWNER_DESC;

private :
	CSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpawner(const CSpawner& rhs);
	virtual ~CSpawner() = default;

public :
	virtual HRESULT				Initialize_Prototype();
	virtual HRESULT				Initialize(void* pArg);

	virtual void				Priority_Update(_float fTimeDelta);
	virtual void				Update(_float fTimeDelta);
	virtual void				Late_Update(_float fTimeDelta);

	void						ADD_SpawnList(_uint iSpawnID, _uint iSpawnCnt);

private :
	CCollider*					m_pColliderCom = { nullptr };
	SPAWNER_TYPE				m_eSpawnerType = { SPAWNER_TYPE::END };
	list<pair<_uint, _uint>>	m_SpawnList = {};

	_float						m_fTimeDelta = {};
#pragma region Time
	TIME_SPANWER_TYPE			m_eTimeSpanwerType = { TIME_SPANWER_TYPE::END };
	_float2						m_SpawnTime = {};
	_uint						m_iLoopCount = {};
#pragma endregion

private :
	HRESULT						Ready_Spawner(SPAWNER_DESC& Desc);
	HRESULT						Ready_Components();

	void						Trigger_Event();
	void						Trigger_SpawnEvent();

public :
	static	CSpawner*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END