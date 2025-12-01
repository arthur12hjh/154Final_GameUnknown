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
	// Desc ¹ÝÈ¯.
	struct Player_Desc* Get_Desc() { return &m_PlayerDesc; }
	const class CBody_Player* Get_BodyPtr() { return m_pBody; }
	_float Get_AnimationRatio();
	void Change_PlayerMode(PLAYER_MODE eMode, PLAYER_STATE eState = PLAYER_STATE::STATE_END); 

	_bool Use_BetaSkill(_uint iSkillID);
	_bool Use_RushSkill();
	void  Set_SuperArmor(_bool bFlag) { m_isSuperArmor = bFlag; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Damaged(void* pArg) override;

private:
	struct Player_Desc		m_PlayerDesc = {};

	class CBody_Player*		m_pBody = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };

	map<PLAYER_MODE, class CPlayerFSM*> m_FSMs = {};
	class CPlayerFSM*		m_pCurrentFSM = { nullptr };
	_float					m_fTime = { 0.f };
	_bool					m_isSuperArmor = { false }; 

	map<_uint, BETA_SKILL_DESC> m_BetaSkills = {};

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