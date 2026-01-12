#pragma once
#include "Character.h"

NS_BEGIN(Engine)
class CAISenceComponent;
class CAIController;
NS_END

NS_BEGIN(Client)
class CUIBase;
class CBullet;
class CNayitbaPartBody;
class CNaytibaLeftWeaponPart;
class CTargetComponent;
class CDropComponent;

struct Character_Skill_Desc;

class CNaytiba final : public CCharacter
{
public :
	typedef struct Nayitba_Desc : GAMEOBJECT_DESC
	{
		_uint						iMonsterID = {};
		_bool						bIsSuperMonster = { false };

		_bool						bIsSpanwer = { false };
		CGameObject*				pTarget = { nullptr };
	}NAYITBA_DESC;

private :
	CNaytiba(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNaytiba(const CNaytiba& Prototype);
	virtual ~CNaytiba() = default;

public:
	virtual HRESULT							Initialize_Prototype() override;
	virtual HRESULT							Initialize(void* pArg) override;

	virtual void							Priority_Update(_float fTimeDelta) override;
	virtual void							Update(_float fTimeDelta) override;
	virtual void							Late_Update(_float fTimeDelta) override;

	virtual HRESULT							Render() override;
	virtual HRESULT							Render_Shadow() override;

	virtual HRESULT							Damaged(void* pArg) override;
	virtual HRESULT							ActionSuccess(void* pArg) override;
	virtual HRESULT							CallNotify(_uint iNotiType, const AnimNotify* pNotify);
	virtual void							Play_Sound(const ANIM_NOTIFY& NotifyReference) override;

	virtual void							RecoveryPoint(RECOVERY_TYPE eRecoveryType, long long iCost = 0);
	virtual void							PlayDeadEffect();
	virtual void							Attack_Interaction(void* pArg = nullptr);

	_uint									GetMonsterID();
	void									Excution();
	CGameObject*							GetTarget();
	
	void									SetVelocity(_bool bIsFlag, _float fVelocity);

	//몬스터의 현재 데이터를 반환
	const NAYTIBA_DESC&						GetMonsterData() { return m_MonsterInfo; }
	
	const CHARACTER_SKILL_DESC*				FindSkillData(_uint iTypeIndex, _uint iSkillIndex);
	const CHARACTER_SKILL_DESC*				GetSkillData(_bool bIsRandom = true, _uint iTypeIndex = -1);
	const _float4x4*						GetLinkTargetBone() { return m_pLinkTargetBoneMatrix; }
	void									SetActive(_bool bIsActive) override;
	void									SetActive() override;
	void									SetActiveMonster(_bool bIsFlag);
	void									Setting_Data(_float fTimeDelta, const NAYITBA_DESC& Desc);
	const NAYTIBA_NETWORK_DESC*				GetStaticMonsterData() { return m_pInitMonsterInfo; }

	void									SetActivePartObject(_bool bIsActive);
	void									SetAttackData(const CHARACTER_SKILL_DESC* pATKDesc);
	void									SetThesholdAction(NAYITBA_EXECUTION_TYPE eExcution);
	void									EnablePhysxController(_bool bEnable);
	void									ResetToBaseState();
	
	_bool									bIsParryHitReaction();
	_bool									bIsRepulseHitReaction();

	NAYITBA_EXECUTION_TYPE					bIsThesholdAction();

	// 몬스터의 이전상태를 반환한다.
	NAYTIBA_STATE							GetMonsterPreState() { return m_MonsterPreState; }

	//레퍼런스 카운트 증가
	CAIController*							GetController();

	virtual void							Active_SFX(const _wstring& strPartTag, const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)override;
	virtual void							Activate_PartObject_Collider(const _wstring& strPartTag, const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef);

private:
	CAISenceComponent*						m_pAISenceCom = { nullptr };
	CTargetComponent*						m_pTargetCom = { nullptr };
	CDropComponent*							m_pDropCom = { nullptr };
	CAIController*							m_pAIController = { nullptr };
	CNayitbaPartBody*						m_pPartBody = { nullptr };
	CNaytibaLeftWeaponPart*					m_pLeftWeapon = { nullptr };

	_uint									m_iMonsterID = {};
	const NAYTIBA_NETWORK_DESC*				m_pInitMonsterInfo = { nullptr };
	const CHARACTER_SKILL_DESC*				m_pAttack_Data = { nullptr };

	NAYTIBA_DESC							m_MonsterInfo = {};
	NAYTIBA_STATE							m_MonsterPreState = {};
	NAYITBA_EXECUTION_TYPE					m_eExcution = { NAYITBA_EXECUTION_TYPE::END };

	_bool									m_bIsSuperMonster = { false };
	_bool									m_bIsTimeVisible = { false };
	_float2									m_vHitVisibleDuration = { 0, 5.f };
	_bool									m_bIsPlaySound = false;
	CUIBase*								m_pStatusUI = { nullptr };

	_uint									m_iComboCount = { };
	_uint									m_iRepulseCount = { };

	_float									m_fHealthBarOffset = {};
	const _float4x4*						m_pLockOnMatrix = { nullptr };
	const _float4x4*						m_pLinkTargetBoneMatrix = { nullptr };

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
	void									VisibleStatusUI(_float fTimeDelta, _bool bIsForce = false);
	
#pragma region Notify Event
	void									CreateHitBox(const AnimNotify* pNotify);
	void									SpawnObject(const AnimNotify* pNotify);
	void									ShootProjectile(const AnimNotify* pNotify);
	void									Attack_Interaction(const AnimNotify* pNotify);
	void									Change_Color(const AnimNotify* pNotify);
	void									Draw_AttackLine(const AnimNotify* pNotify);
	void									Play_SFXEffect(const AnimNotify* pNotify);
	void									Play_MoveSound(_uint iType);

	void									Play_StatueMoveSound(_uint iType, _float fRandomIndex);
	void									Play_BanacleMoveSound(_uint iType);
	void									Play_AntlionMoveSound(_uint iType);
	void									Play_GorillaMoveSound(_uint iType, _uint SoundType);

	_bool									Compare_SFX_Name(const string& szSFXName);
#pragma endregion

#pragma region Damage Logic
	// 기본적인 데미지 연산
	_bool							ActionDamageLogic(const DEFAULT_DAMAGE_DESC* pDamageDesc);
	// 방어 타입에 대한 데미지 연산
	_bool							DefenseTypeDamage(const DEFAULT_DAMAGE_DESC* pDamageDesc, _float fDamageReductionRate = 0.1f);
#pragma endregion

public:
	static	CNaytiba*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
NS_END