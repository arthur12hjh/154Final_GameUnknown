#pragma once
#include "Character.h"

NS_BEGIN(Engine)
class CAISenceComponent;
class CAIController;
NS_END

NS_BEGIN(Client)
class CUIBase;
class CBullet;
class CTargetComponent;
class CDropComponent;

struct Character_Skill_Desc;

class CNayitba final : public CCharacter
{
public :
	typedef struct Nayitba_Desc : GAMEOBJECT_DESC
	{
		_uint						iMonsterID = {};
		_bool						bIsSuperMonster = { false };
	}NAYITBA_DESC;

private :
	CNayitba(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNayitba(const CNayitba& Prototype);
	virtual ~CNayitba() = default;

public:
	virtual HRESULT							Initialize_Prototype() override;
	virtual HRESULT							Initialize(void* pArg) override;

	virtual void							Priority_Update(_float fTimeDelta) override;
	virtual void							Update(_float fTimeDelta) override;
	virtual void							Late_Update(_float fTimeDelta) override;

	virtual HRESULT							Render() override;
	 
	virtual HRESULT							Damaged(void* pArg) override;
	virtual HRESULT							ActionSuccess(void* pArg) override;
	virtual HRESULT							CallNotify(_uint iNotiType, const AnimNotify* pNotify);
	virtual void							RecoveryPoint(RECOVERY_TYPE eRecoveryType, long long iCost = 0);
	virtual void							Set_Dead(_bool isDead);

	_uint									GetMonsterID();
	CGameObject*							GetTarget();

	const NAYTIBA_NETWORK_DESC*				GetStaticMonsterData() { return m_pInitMonsterInfo; }
	
	//몬스터의 현재 데이터를 반환
	const NAYTIBA_DESC&						GetMonsterData() { return m_MonsterInfo; }
	
	const CHARACTER_SKILL_DESC*				FindSkillData(_uint iTypeIndex, _uint iSkillIndex);
	const CHARACTER_SKILL_DESC*				GetSkillData(_bool bIsRandom = true, _uint iTypeIndex = -1);
	void									SetAttackData(const CHARACTER_SKILL_DESC* pATKDesc);

	_bool									bIsHitReaction();
	// 몬스터의 이전상태를 반환한다.
	NAYTIBA_STATE							GetMonsterPreState() { return m_MonsterPreState; }

	//레퍼런스 카운트 증가
	CAIController*							GetController();
	virtual void							Active_SFX(const _wstring& strPartTag, const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)override;

private:
	CAISenceComponent*						m_pAISenceCom = { nullptr };
	CTargetComponent*						m_pTargetCom = { nullptr };
	CDropComponent*							m_pDropCom = { nullptr };
	CAIController*							m_pAIController = { nullptr };

	_uint									m_iMonsterID = {};
	const NAYTIBA_NETWORK_DESC*				m_pInitMonsterInfo = {};
	const CHARACTER_SKILL_DESC*				m_pAttack_Data = { nullptr };

	NAYTIBA_DESC							m_MonsterInfo = {};
	NAYTIBA_STATE							m_MonsterPreState = {};

	_bool									m_bIsSuperMonster = { false };
	_float2									m_vHitVisibleDuration = { 0, 5.f };
	_bool									m_bIsTimeVisible = { false };
	CUIBase*								m_pStatusUI = { nullptr };

	_uint									m_iComboCount = { };
	const _float4x4*						m_pLockOnMatrix = { nullptr };
	const _float4x4*						m_pHeadBoneMatrix = { nullptr };


	// 이거는 랜덤안하면 순차적으로 증가하면서 나오는 공격에 대한 인덱스
	size_t									m_iSkillIndex = {};

	size_t									m_iNumCandidate = {};
	vector<const CHARACTER_SKILL_DESC*>		m_SkillCandidates = {};
	vector<CBullet*>						m_pBulletList = {};

private :
	HRESULT									Ready_CharacterData();

	HRESULT									ADD_Components();
	HRESULT									ADD_PartObjects();

	void									BattleEvent(CGameObject* pTarget, NAYTIBA_STATE eState);

	void									VisibleStatusUI(_float fTimeDelta);
	
#pragma region Notify Event
	void									CreateHitBox(const AnimNotify* pNotify);
	void									SpawnObject(const AnimNotify* pNotify);
	void									ShootProjectile(const AnimNotify* pNotify);
#pragma endregion

#pragma region Damage Logic
	// 기본적인 데미지 연산
	_bool							ActionDamageLogic(const DEFAULT_DAMAGE_DESC* pDamageDesc);
	// 방어 타입에 대한 데미지 연산
	_bool							DefenseTypeDamage(const DEFAULT_DAMAGE_DESC* pDamageDesc, _float fDamageReductionRate = 0.1f);
#pragma endregion

public:
	static	CNayitba*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
NS_END