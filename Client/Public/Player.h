#pragma once

#include "Character.h"

NS_BEGIN(Engine)
class CCollider;
class CNavigation;
class CCharacterController;
NS_END

NS_BEGIN(Client)

class CPlayer final : public CCharacter
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	// Desc 반환.
	virtual void Handle_Notify(void* pArg);
	struct Player_Desc* Get_Desc() { return &m_PlayerDesc; }
	_float Get_AnimationRatio();

	_bool Use_BetaSkill(_uint iSkillID);
	_bool Use_RushSkill();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Damaged(void* pArg) override;

public:
	// 테스트용 로직들이라 한군데 모아놧습니다 
	// 나중에 한번에 정리할게요.
	void Update_TestLogic(_float fTimeDelta);
	_float m_fTestTimer = { 0.f };

private:
	struct Player_Desc		m_PlayerDesc = {};
	CCollider*				m_pColliderCom = { nullptr };
	class CPlayerFSM*		m_pFSM = { nullptr };
	map<_uint, BETA_SKILL_DESC> m_BetaSkills = {};

	//
	_float					m_fTime = { 0.f };

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_PlayerDesc();
	HRESULT Ready_BetaSkillDesc();
	HRESULT Ready_FSM();

private:
	void Update_FSM(_float fTimeDelta);
	void Update_RushSkill(_float fTimeDelta);
	void Update_BetaSkill();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END