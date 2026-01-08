#pragma once

#include "Character.h"

NS_BEGIN(Engine)
class CCollider;
class CNavigation;
class CCharacterController;
class CMotionTrailComponent;
NS_END

NS_BEGIN(Client)
class CWeapon;

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


	virtual void	 Active_SFX(const _wstring& strPartTag, const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)override;
	virtual void	 Activate_PartObject_Collider(const _wstring& strPartTag, const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef);
	virtual HRESULT	 CallNotify(_uint iNotiType, const AnimNotify* pNotify);

	class CPlayerFSM* Get_PlayerFSM() { return m_pFSM; }
	void MotionTrailEnable(_bool bIsEnable);
	void MotionTrailRimLight(_float fRimLightPower, _float fRimLightIntensity, _float4 vColor);
	void MotionTrailCoolDown(_float fCoolDown);

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;

	virtual void		Priority_Update(_float fTimeDelta) override;
	virtual void		Update(_float fTimeDelta) override;
	virtual void		Late_Update(_float fTimeDelta) override;

	virtual HRESULT		Render() override;
	virtual HRESULT		Render_Shadow() override;
	virtual HRESULT		Render_MotionBlur() override;
	virtual HRESULT		Damaged(void* pArg) override;
	virtual void		RecoveryPoint(RECOVERY_TYPE eRecoveryType, long long iCost = 0);
	virtual void		Attack_Interaction(void* pArg = nullptr);

	void				SetSkillDataID(_uint iSkillID);
	_int				GetSkillDataID();

public:
	// 테스트용 로직들이라 한군데 모아놧습니다 
	// 나중에 한번에 정리할게요.
	void Update_TestLogic(_float fTimeDelta);
	_float m_fTestTimer = { 0.f };
	_float m_fShieldTimer = { 0.f };
private:
	struct Player_Desc			m_PlayerDesc = {};
	CCollider*					m_pColliderCom = { nullptr };
	CWeapon*					m_pWeapon = { nullptr };
	class CCameraBone_Player*	m_pCameraBone = { nullptr };
	class CPlayerFSM*			m_pFSM = { nullptr };
	
	map<_uint, BETA_SKILL_DESC> m_BetaSkills = {};

	//
	_float					m_fTime = { 0.f };
	_int					m_iSkillID = {};
private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_PlayerDesc();
	HRESULT Ready_BetaSkillDesc();
	HRESULT Ready_FSM();

private:
	void Update_FSM(_float fTimeDelta);
	void Update_RushSkill(_float fTimeDelta);
	void Update_BetaSkill(_float fTimeDelta);
	void Update_Interaction(_float fTimeDelta);
	void Update_PotionUse(_float fTimeDelta);
	void Update_LinkAttack(_float fTimeDelta);
	//블링크, 리펄스, 저회 키 선입력용.
	void Update_ReactionSkills(_float fTimeDelta); 
	void Update_ReactionSkillInput(_float fTimeDelta);
	void Handle_Hit(DEFAULT_DAMAGE_DESC* pDamageDesc, const CHARACTER_SKILL_DESC* pSkillDesc);
	void Calc_Damage(DEFAULT_DAMAGE_DESC* pDamageDesc, const CHARACTER_SKILL_DESC* pSkillDesc);

	void CreateHitBox(const AnimNotify* pNotify);

private:
	void Execution_Nayitba();
	void LinkAttack_Nayitba(const NAYITBA_EXECUTION_TYPE& eLinkAttackType);
	
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END