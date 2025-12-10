#pragma once
#include "Engine_Defines.h"
#include "Transform.h"
#include "CharacterController.h"

using namespace std; 

namespace Engine
{
	class CGameObject;
	class CTransform;
	class CCharacterController;
}

namespace Client
{
	enum class RECOVERY_TYPE
	{
		RECOVERY_HP,		// HP 회복
		RECOVERY_SHILED,	// SHILED 회복
		RECOVERY_STEMINA,	// STEMINA 회복
		RECOVERY_END
	};

	enum class INTERACTION_STATE {
		DEFAULT,	// 그냥 아무것도 안하고 아무일도 없을때 나올 녀석
		CONTACT,	// 접촉해서 사용작용 가능한 녀석
		LOCK,		// 상호작용 할수없음
		ACTIVE,		// 상호작용 하는중
		END			// 더이상 앞으로도 네버 상호작용 불가능
	};

	typedef struct Default_Status
	{
		long long						iCurrentHealth;
		long long						iCurrentShield;
	}DEFAULT_STATUS;


	// 기본적인 밖에서 저장하거나 불러오는 캐릭터 구조체
	// 기본세팅 요거
	typedef struct Character_NetWork_Desc
	{
		long long		iMaxHealth;
		long long		iMaxShield;
		long long		iMaxBetaEnergy;
		
		long long		iAttackPoint;
		long long		iShieldAttackPoint;

		float			fCriticalPercent;
		float			fCriticalDamage;
		float			fLinkAttackApplyDamage;

	}CHARACTER_NETWORK_DESC;

	// 인게임에서 실질적으로 사용되는 캐릭터 구조체
	enum class PLAYER_MODE { IDLE, BATTLE, LOCKON, END };
	enum class PLAYER_STATE { 
		IDLE, WALK_START, WALK, WALK_END, JUMP,LIGHT_ATTACK , 
		EVADE, LANDING, VENDING_INTERACTION,
		HIT, 
		BETA_CHARGINGSLASH, BETA_TRIPLET,
		
		PARRY, PARRY_SUCCESS, PARRY_END, PARRY_GUARD,

		AERIAL_ATTACK, //미구현
		
		DRAW_HAIRPIN, SHEATHE_HAIRPIN,

		STATE_END
	};

	enum class SKILL_STATE {
		DEFAULT,   // 비활성화
		ACTIVE_ON, // 딱 활성화 됐을 때
		ACTIVE,	   // 활성화 상태
		USE,	   // 스킬 쓴 순간
		END,	   //
	};

	typedef struct Player_Desc : public DEFAULT_STATUS
	{
		long long						iMaxHealth;
		long long						iMaxShield;
		//베타 에너지 최대치
		long long						iMaxBetaEnergy;

		//현재 베타 에너지
		long long						iCurrentBetaEnergy;

		long long						iCurrentAttackPoint;
		long long						iCurrentShieldATK;

		float							fCurrentCTPercent;
		float							fCurrentCTDamage;
		float							fCurrentLinkApplyDamage;

		int								iCurrentPotions; // 현재 소지한 포션 개수
		int								iMaxPotions; // 전체 포션 개수

		//
		SKILL_STATE						eRushState;					// 러쉬 활성화 여부	
		float							fMaxRushCoolTime;			// 러쉬 전체 쿨타임
		float							fCurrentRushCoolTime;		// 러쉬 현재 쿨타임

		bool							isSuperArmor = { false };
		bool							isLookFixed = { false };

		// 혹시 몰라서 플레이어 데스크에 떄려박앗습니다
		// 플레이어의 락온모드 전환 거리
		float							fLockOnDistance = { 30.f };
		// 플레이어의 배틀모드 전환 거리 
		float							fBattleDistance = { 40.f };
		// 베타스킬 상태들
		unsigned int					iBetaSkillId[4];			// 사용중인 스킬ID,
		SKILL_STATE						eBetaSkillState[4];
		unsigned int					iBetaSkillCount;			// 현재 활성화된 스킬 갯수. 최대 4개

		// 플레이어의 전투 상태. battle, idle, lockon
		PLAYER_MODE						ePlayerMode = { PLAYER_MODE::IDLE };
		class CTransform* pPlayerTransform = { nullptr };
		class CCharacterController* pPlayerController = { nullptr };

		bool   isRequestLockonToggle = { false };
		float  fCurrentMinDist = { FLT_MAX };
		float  fModeTimer = { 0.f };
		bool   HasTarget = { false };
		bool   isBossLock = { false };
		// idle일땐 당연히 안보이고, 무기 스왑 애니메이션에서
		// 해당 값 제어 해서 무기가 보일지, 비녀가 보일지 결정해줄 것
		bool   isWeaponVisible = { false };

		// 패링 판정이 가능한지 체크하는 변수.
		// 만약 패링이 가능하다면 true, 불가능하다면 False
		bool   isParryable = { false };
		// 저스트 패링 판정이 가능한지 체크하는 변수.
		// 만약 저스트 패링이 가능하다면 true, 불가능하다면 false
		bool   isJustParryable = { false };

	}PLAYER_DESC;

	// 스킬 구조체
	// 이건 경우에 따라 사용 할수도 있음
	enum class ATTACK_DIRECTION
	{
		ATK_LEFT,
		ATK_RIGHT,
		ATK_UP,
		ATK_DOWN,
		END
	};

	// Skill Type
	enum class SKILL_TYPE { 
		DEFAULT_SKILL,			// Default Attack
		INTERACTION_SKILL,		// Interaction Skill
		BETA_SKILL,				// Beta Skill
		ALPHA_SKILL,			// Alpha Skill
		MIMESIS_SKILL,			// Mimesis Skill
		END
	};
	
	// enum class 비트 연산 지원안해서 바꿈
	enum SKILL_PROPERTY : UINT8
	{
		// Bit Mask ex 
		// All Property : 31
		// Parry & Eavde : 3
		PARRYABLE		= 0b00000001, // 1  <- 패링가능
		EVADEABLE		= 0b00000010, // 2  <- 회피 가능
		BLINKABLE		= 0b00000100, // 4  <- 블링크 가능
		SUPERARMOR		= 0b00001000, // 8  <- 슈퍼아머
		EXCUTION		= 0b00010000, // 16 <- 처형
		GUARD			= 0b00100000, // 32 <- 가드
		PARRY			= 0b01000000, // 64 <- 가드
		END
	};



	typedef struct Character_Skill_Desc
	{
		unsigned int				iSkillID;					// SKILL ID
		char						szAnimationName[256];		// Play Anim Name

		// Default : None
		// Anim Able : Anim Name
		char						szHitAnimationName[256];	// Hit Anim Name
		char						szLinkBoneName[256];

		// SKill Damage
		long long					iSkillDamage;				// 스킬 데미지

		// Skill Range
		_float						fRange;						// 스킬 사거리	

		// Skill Hit Box Size
		_float3						vHitBoxExtents;				// 스킬 히트박스 크기
		_uint						iMaxComboCount;


		// Attack Dir
		ATTACK_DIRECTION			eATK_Direction;				// 공격 방향 UP DOWN LEFT RIGHT

		// SKILL TYPE
		// BeatSKill, AlphaSKill... 
		SKILL_TYPE					eSkillType;					// 스킬 타입

		// Property
		//		PARRYABLE, EVADEABLE, BLINKALBE, SUPERARMOR...
		SKILL_PROPERTY				eProPerty;					// 스킬 속성
 
	} CHARACTER_SKILL_DESC;

	typedef struct tagPlayerBetaSkillDesc : public CHARACTER_SKILL_DESC
	{
		//요구되는 베타스킬 게이지 량
		_uint iRequiredBetaGauge;
	} BETA_SKILL_DESC;

	// 몬스터 구조체
	// 인게임용
	// ELITE, ELDER가 보스
	enum class NAYTIBA_TYPE { MINION, WARRIOR, ELITE, ELDER, END};
	enum class AI_TYPE { PASSIVE, AGGRESSIVE, DEFENSIVE, END };
	typedef struct Naytiba_NetWork_Desc
	{
		_uint				iMonsetID;
		unsigned int		iNumPhase;

		char				szAnimationName[256];
		char				szMonsterName[256];
		NAYTIBA_TYPE		eNaytiba_Type;
		AI_TYPE				eAI_Type;

		char				szModelPrototype[256];
		char				szAIControllerPrototype[256];
		char				szAIBehaviorPrototype[256];

		long long			iMaxHealth;
		long long			iMaxShield;
		_uint				iMaxStamina;
		float				fMoveSpeed;

		float				fAttackCoolTime;
		float				fAttackRange;
		_float3				fColliderExtents;

		//여기서 사용하는 스킬 정보
		vector<_uint>		iAttackList;
	}NAYTIBA_NETWORK_DESC;

	// 인게임용
	enum class NAYTIBA_STATE { DEFAULT, MIMESSIS, BATTLE, DEAD, END };
	enum class COMBAT_ATTRIBUTE { SUPER_ARMOR, EVASION, END };
	typedef struct Naytiba_Desc : public DEFAULT_STATUS
	{
		unsigned int		iCurrentPhase;
		_uint				iCurrentStamina;

		_float3				vLockOnPoint;
		_float3				vStatusBarPoint;

		_float2				fAttackCoolTime;
		_float				fAttackRange;
		_float				fMoveSpeed;

		_bool				m_bIsParryHitAble;
		NAYTIBA_STATE		eNaytibaState;
		COMBAT_ATTRIBUTE	eCombatAttribute;
		vector<const CHARACTER_SKILL_DESC *>	iAttackList[ENUM_CLASS(SKILL_TYPE::END)];
	}NAYTIBA_DESC;

	// 만약에 공격 타입같은거도 나눌거면 여기서 나눠서 사용하세요

	enum class QUEST_STATE { LOCKED, AVAILABLE, ACTIVE, COMPELETED, END };
	typedef struct QuestStruct
	{
		// 기본구조체 대충 여기서 퀘스트 타입도 만들려면 만들기
		QUEST_STATE			QuestState;
		_uint				iQuestID;

		vector<_uint>		RewardLists;
	}QUEST_STRUCT;

	enum class ITEM_TYPE { EQUIPMENT, CONSUMEABLE, MATERIAL, QUEST, END };

	typedef struct ItemDefaultDesc
	{

	}ITEM_DEFAULT_DESC;

	typedef struct ConsumeAbleItemDesc
	{
		long long				iRecoveryData;
		char					ToolTip[MAX_PATH];
	}CONSUMABLE_ITEM_DESC;

	typedef union Item_Properity_Desc
	{
		ITEM_DEFAULT_DESC			DefaultData;
		CONSUMABLE_ITEM_DESC		ConsumAble_Data;
	}ITEM_PROPERITY_DESC;

	typedef struct Item_Desc
	{
		ITEM_TYPE					eType;
		_uint						iItemID;
		
		// 타입에 따라서 달라지는 데이터입니다.
		// 이거 타입별로 캐스팅 다르게 해주세요
		// 일단 임시로 두개만해뒀는데 다른건 매번 추가해 주세요
		// 
		// ITEM		: CONSUMABLE_ITEM_DESC
		// 나머지	: ITEM_DEFAULT_DESC
		ITEM_PROPERITY_DESC			Propertiy_Data;
	}ITEM_DSEC;

	typedef struct Default_Damage_Desc
	{
		CGameObject*		pAttacker;
		_float3				vHitPoint;
		_float3				vHitDir;

		_float4x4			vHitWorldMatrix;
		
		_float3				vImpactDir;
		_float				fImpactForce;
		_bool				bIsHitMotion;

		const void*			pSkillData;
	}DEFAULT_DAMAGE_DESC;


	typedef struct Interaction_Data
	{
		_uint				iID;
		char				szObjectTag[256];
		char				szInteractionText[256];
		_float3				vUIPivot;

	}INTERACTION_DATA;
}