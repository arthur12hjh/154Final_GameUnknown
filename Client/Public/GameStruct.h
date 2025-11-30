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
		IDLE, WALK, WALK_END, JUMP,LIGHT_ATTACK , 
		EVADE, LANDING, VENDING_INTERACTION,
		HIT, BETA_CHARGINGSLASH, AERIAL_ATTACK, //미구현

		STATE_END
	};
	typedef struct Player_Desc
	{
		long long				iMaxHealth;
		long long				iMaxShield;
		long long				iMaxBetaEnergy;

		long long				iCurrentHealth;
		long long				iCurrentShield;
		long long				iCurrentBetaEnergy;

		long long				iCurrentAttackPoint;
		long long				iCurrentShieldATK;

		float					fCurrentCTPercent;
		float					fCurrentCTDamage;
		float					fCurrentLinkApplyDamage;

		int						iCurrentPotions; // 현재 소지한 포션 개수
		int						iMaxPotions; // 전체 포션 개수

		//플레이어의 전투 상태. battle, idle, lockon
		PLAYER_MODE				ePlayerMode;
		class CTransform*		pPlayerTransform = { nullptr };
		class CCharacterController* pPlayerController = { nullptr }; 
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

		// Attack Dir
		ATTACK_DIRECTION			eATK_Direction;				// 공격 방향 UP DOWN LEFT RIGHT

		// SKILL TYPE
		// BeatSKill, AlphaSKill... 
		SKILL_TYPE					eSkillType;					// 스킬 타입

		// Property
		//		PARRYABLE, EVADEABLE, BLINKALBE, SUPERARMOR...
		SKILL_PROPERTY				eProPerty;					// 스킬 속성
	}CHARACTER_SKILL_DESC;


	// 몬스터 구조체
	// 인게임용
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
		float				fMoveSpeed;

		float				fAttackCoolTime;
		float				fAttackRange;
		_float3				fColliderExtents;

		//여기서 사용하는 스킬 정보
		vector<_uint>		iAttackList;
	}NAYTIBA_NETWORK_DESC;

	// 인게임용
	enum class NAYTIBA_STATE { DEFAULT, MIMESSIS, BATTLE, END };
	enum class COMBAT_ATTRIBUTE { SUPER_ARMOR, EVASION, END };
	typedef struct Naytiba_Desc
	{
		unsigned int		iCurrentPhase;

		long long			iCurrentHealth;
		long long			iCurrentShield;

		_float2				fAttackCoolTime;
		_float				fAttackRange;
		_float				fMoveSpeed;

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

	typedef struct Default_Damage_Desc
	{
		CGameObject*		pAttacker;
		_float3				vHitPoint;
		_float3				vHitDir;

		_float4x4			vHitWorldMatrix;
		
		_float3				vImpactDir;
		_float				fImpactForce;

		const void*			pSkillData;
	}DEFAULT_DAMAGE_DESC;
}