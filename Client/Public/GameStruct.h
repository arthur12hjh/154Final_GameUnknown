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

#define iFLAG_INTERPOLATION_NONE		0
#define iFLAG_INTERPOLATION_LERP		(1 << 0)

#define iFLAG_CAMERA_SOURCE_BONE        (1 << 0)
#define iFLAG_CAMERA_SOURCE_WORLD       (1 << 1)
#define iFLAG_CAMERA_SOURCE_TARGET      (1 << 2)

#define iFLAG_CAMERA_MOVEDIRECT         (1 << 8)
#define iFLAG_CAMERA_MOVELERP           (1 << 9)

#define iFLAG_CAMERA_LOOKAT_PIVOT       (1 << 16)
#define iFLAG_CAMERA_LOOKAT_TARGET      (1 << 17)


	// 특정 상호작용에 대한 타입
	enum class ATK_INTERACTION_TYPE { PERFECT_DOGE, BLINK, REPULSE, END };

	enum class RECOVERY_TYPE
	{
		RECOVERY_HP,		// HP 회복
		RECOVERY_SHILED,	// SHILED 회복
		RECOVERY_STEMINA,	// STEMINA 회복
		RECOVERY_END
	};

	typedef struct Default_Status
	{
		long long						iCurrentHealth;
		long long						iCurrentShield;
	}DEFAULT_STATUS;

	typedef struct tagRimLightDesc
	{
		_float4		vRimLightColor = { 1.f, 1.f, 1.f, 1.f };
		_float		fRimLightPower = 2.f;
		_float		fRimLightIntensity = 1.f;
	} RIMLIGHT_DESC;


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
		IDLE, WALK_START, WALK, WALK_END, JUMP, LIGHT_ATTACK,
		EVADE, LANDING, SPRINT, SPRINT_END,

		// 자판기
		VENDING_INTERACTION, 
		// 발로 차서 까는 박스
		SUPPLYBOX_INTERACTION, 
		// 시체
		CORPSE_INTERACTION,

		//피격 & 그랩
		HIT, GRAB,
		
		BETA_CHARGINGSLASH, BETA_TRIPLET,

		PARRY, PARRY_SUCCESS, PARRY_END, PARRY_GUARD,

		AERIAL_ATTACK, //미구현

		// 링크 어택 모음
		GIGAS_LINKATTACK, SCARLET_LINKATTACK, SCARLET_PHASE2_LINKATTACK,

		DRAW_HAIRPIN, SHEATHE_HAIRPIN,

		JUST_EVADE, 
		REPULSE, 
		//블링크는 시작 모션 & 공격 모션 둘 나눠서 제어.
		BLINK_START, BLINK_ATTACK,

		//여기에 상태들 다 Enum화 해서 올려놔야해.
		//안쓸것들은 나중에 치워줘
		TEST_STATE,

		STATE_END
	};
	enum class PLAYER_DIRECTION { STRAIGHT, LEFT, RIGHT, BACKWARD, END };

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
		float							fPotionCoolDown = { 3.f };   // 포션 사용 쿨타임
		float							fCurrentPotionCoolDown = { 0.f }; // 현재 남은 쿨타임
		//
		SKILL_STATE						eRushState;					// 러쉬 활성화 여부	
		float							fMaxRushCoolTime;			// 러쉬 전체 쿨타임
		float							fCurrentRushCoolTime;		// 러쉬 현재 쿨타임

#pragma region REACTION_SKILL
		bool							isReactionSkillUsable = { false };
		ATK_INTERACTION_TYPE			eReactionType = { ATK_INTERACTION_TYPE::END };
		//선판정이 들어갈 수 있는, 남은 프레임 수.
		int								iLeftReactionSkillFrameAcc = { 0 };
#pragma endregion

		// 무적
		bool							isInvincible = { false }; 
		// 슈퍼아머
		bool							isSuperArmor = { false };
		// 룩벡터 고정.
		bool							isLookFixed = { false };
		bool							isInteracting = { false };
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
		bool   isLockChangable = { true };
		// idle일땐 당연히 안보이고, 무기 스왑 애니메이션에서
		// 해당 값 제어 해서 무기가 보일지, 비녀가 보일지 결정해줄 것
		bool   isWeaponVisible = { false };

		// 패링 판정이 가능한지 체크하는 변수.
		// 만약 패링이 가능하다면 true, 불가능하다면 False
		bool   isParryable = { false };
		// 저스트 패링 판정이 가능한지 체크하는 변수.
		// 만약 저스트 패링이 가능하다면 true, 불가능하다면 false
		bool   isJustParryable = { false };

		bool   isLinkAttackAvailable = { true };
		class CNaytiba* pLinkAttackTarget = { nullptr };
		const _float4x4* pGrabBone = { nullptr };
		class CGameObject* pGrabAttackter = { nullptr };
		bool isGrabbed = { false };
		// 골드
		int iOwnGold{ 0 };
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
		DEFAULT_SKILL,	
		INTERACTION_SKILL, // 무조건 그랩이라고 보면 된다.
		BETA_SKILL,		
		ALPHA_SKILL,	
		MIMESIS_SKILL,	
		SECOND_PHASE_SKILL,
		BLINK_SKILL,			
		REPULSE_SKILL,
		END
	};

	// enum class 비트 연산 지원안해서 바꿈
	enum SKILL_PROPERTY : UINT8
	{
		// Bit Mask ex 
		// All Property : 31
		// Parry & Eavde : 3
		PARRYABLE = 0b00000001, // 1  <- 패링가능
		EVADEABLE = 0b00000010, // 2  <- 회피 가능
		BLINKABLE = 0b00000100, // 4  <- 블링크 가능
		SUPERARMOR = 0b00001000, // 8  <- 슈퍼아머
		EXCUTION = 0b00010000, // 16 <- 처형
		GUARD = 0b00100000, // 32 <- 가드
		PARRY = 0b01000000, // 64 <- 패링
		IGNORE_GUARDBREAK = 0b10000000, // 128 <- 가드 파괴
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
		_uint						iMaxRepulseCount;

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
	enum class NAYTIBA_TYPE { MINION, WARRIOR, ELITE, ELDER, END };
	enum class AI_TYPE { PASSIVE, AGGRESSIVE, DEFENSIVE, END };
	typedef struct Naytiba_NetWork_Desc
	{
		_uint				iMonsetID;
		unsigned int		iNumPhase;

		char				szAnimationName[256];
		char				szMonsterName[256];

		char				szHeadBoneName[256];
		char				szLeftWeaponPrototypeName[256];
		char				szLeftBoneName[256];

		char				szRightWeaponPrototypeName[256];
		char				szRightBoneName[256];
		char				szFaceName[256];

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
		float				fLerpRatio;
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

		NAYTIBA_STATE		eNaytibaState;
		COMBAT_ATTRIBUTE	eCombatAttribute;
		vector<const CHARACTER_SKILL_DESC*>	iAttackList[ENUM_CLASS(SKILL_TYPE::END)];
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
		CGameObject* pAttacker;
		_float3				vHitPoint;
		_float3				vHitDir;

		_float4x4			vHitWorldMatrix;

		_float3				vImpactDir;
		_float				fImpactForce;
		_bool				bIsHitMotion;

		const void* pSkillData;
	}DEFAULT_DAMAGE_DESC;

	//인터랙션 타입.
	enum class INTERACTION_TYPE { ITEM, SUPPLY_BOX, VENDING_MACINE, CHAIR, DOOR, LIFT_CONTROLLER, CORPSE, NPC, END };
	//인터랙션 상태(상호작용 중, 닿았는지 등)
	enum class INTERACTION_STATE {
		DEFAULT,	// 그냥 아무것도 안하고 아무일도 없을때 나올 녀석
		CONTACT,	// 접촉해서 사용작용 가능한 녀석
		LOCK,		// 상호작용 할수없음
		ACTIVE,		// 상호작용 하는중
		END			// 더이상 앞으로도 네버 상호작용 불가능
	};

	typedef struct Interaction_Data
	{
		_uint				iID;
		_wstring			szObjectTag;
		_wstring			szInteractionText;
		_float3				vUIPivot;
		_float				fInteractionTime;
		INTERACTION_TYPE	eType;
	}INTERACTION_DATA;

	typedef struct Script_Data
	{
		_wstring			szScriptText;
		_float4				vColor{ 1.f, 1.f, 1.f, 1.f };
	}SCRIPT_DATA;

	typedef struct Script_Desc
	{
		_wstring szScriptTag{};
		_wstring szAnimTag{};
		_wstring szSpeaker{};
		_bool bCanControl{ false };
		_float2 vInitOffset{ 0.f, 0.f };
		vector<SCRIPT_DATA> Scripts;
	}SCRIPT_DESC;

			

	// Camera_Action, Camera_CutScene 전용 Json Data
	///
	/// 3자리 카메라
	/// 11n : 플레이어 기본 카메라 액션
	///  - 111 : 리펄스
	///  - 112 : 블링크
	/// 12n : Link Attack. 무력화 공격 카메라 액션
	///  - 123 : 기가스 링크 어택
	///  - 121 : 홍련 1페이즈 링크 어택
	///  - 122 : 홍련 2페이즈 링크 어택 카운터
	/// 13n : 
	/// 14n : 잡기 카메라 액션
	///  - 145 : 홍련 잡기
	/// 
	/// 4자리 카메라
	/// 110n : 도로롱 조우
	/// 111n : 기가스 조우
	/// 112n : 기가스 처형
	/// 120n : 사막에서 홍련 조우
	/// 125n : 마을에서 홍련 이별
	/// 13nn : 홍련 조우
	/// 14nn : 홍련 페이즈 변경
	/// 15nn : 홍련 처형
	/// 
	typedef struct CameraTrack_Desc
	{
		_float						fTrackPosition;						// 실행 시간.
		_float3						vTrackValue;						// 목표 변환값
		_float						fTangentStart;						// 에르미트 보간법 시작 각도
		_float						fTangentEnd;						// 에르미트 보간법 종료 각도
		_uint						iInterpolationFlag;					// 플래그
	}CAMERA_TRACK_DESC;

	typedef struct CameraAnimation_Data
	{
		_uint						iCameraAnimationID;					// 카메라 ID
		_uint						iCameraAnimationFlag;				// 카메라 플래그(본부착, 전환시 보간여부 등)
		_float						fBaseFOV;							// 기본 FOV
		_float3						vBaseCameraPivot;					// 기본 카메라 LookAt 포인트
		_float3						vBaseBonePosition;					// 카메라본 기본 위치(플래그 활성 시)
		_float3						vBaseBoneRotation;					// 카메라본 기본 각도(플래그 활성 시)
		_char						szCameraAnimationName[MAX_PATH];	// 카메라본 애니메이션 이름
		vector<CAMERA_TRACK_DESC>	FOVTrackList;						// FOV 채널
		vector<CAMERA_TRACK_DESC>	PivotTrackList;						// 카메라 LookAt 포인트 채널
		vector<CAMERA_TRACK_DESC>	BonePositionTrackList;				// 카메라본 위치 채널
		vector<CAMERA_TRACK_DESC>	BoneRotationTrackList;				// 카메라본 각도 채널
	}CAMERA_ANIMATION_DATA;


	// Cinematic 관련 데이터들
	enum class CINEMATICNODE_STATE { 
		ACTIVE_CINEOBJ,			// 0 
		PLAY_CINEOBJ,			// 1
		ACTIVE_CHARACTER,		// 2
		ACTIVE_CAMERA,			// 3
		DEACTIVE_CHARACTER,		// 4
		DEACTIVE_CAMERA,		// 5
		FADE_IN,				// 6
		FADE_OUT,				// 7
		PLAY_SOUND,				// 8
		MOVE_CHARACTER,			// 9
		MOVE_NPC,				// 10
		CREATE_TRIGGERBOX,		// 11
		CREATE_TARGETLIGHT,		// 12
		END };

	typedef struct Cinematic_Index_Desc
	{
		_uint iActiveIndex;
		_char szObjectTag[MAX_PATH];
		_float3 vPosition;
		_float4 vRotation;		// w = 0이면 그냥 회전. w = 1이면 LookAt
		_float3 vScale;
		_uint iInputInt;
		_float fInputFloat;
	}CINEMATIC_INDEX_DESC;

	typedef struct Cinematic_Node_Desc
	{
		CINEMATICNODE_STATE eState;
		_float fTrackPosition;
		_char szObjectTag[MAX_PATH];
		_uint iActiveIndex;
		vector<CINEMATIC_INDEX_DESC> CinematicIndexDataList;
	}CINEMATIC_NODE_DESC;

	typedef struct Cinematic_Desc
	{
		_uint iCinematicID;
		_char szCinematicName[MAX_PATH];
		vector<CINEMATIC_NODE_DESC> CinematicNodeTrackList;
	}CINEMATIC_DESC;

	typedef struct Attack_Interaction_Desc
	{
		// 공격에대한 상호작용을 하기위해 넘어 오는 구조체
		ATK_INTERACTION_TYPE	eInteraction_Type;

		// 판정 시간( 프레임 단위로 판정한다. )
		_uint					iFrameCnt;

		// 필요한경우 pArg 넘겨서 받아오기
		// 근데 이거 노티라서 노티넘겨줄거같긴함
		void*					pArg;
	}ATK_INTERACTION_DESC;

	enum class CINEMATICOBJECT_TYPE { CINEMATICOBJECT, ACTIONCAMERA, END };
	typedef struct Level_CinematicObject_Desc
	{
		_char szObjectTag[MAX_PATH];
		CINEMATICOBJECT_TYPE eType;
		_float3 vPosition;
		_float4 vRotation;
		_float3 vScale;
	}LEVEL_CINEMATICOBJECT_DESC;

	typedef struct Npc_Data_Desc
	{
		_uint						iNpcID;
		char						szName[MAX_PATH];
		char						szPrototypeModel[MAX_PATH];
		char						szAIController[MAX_PATH];

		_float3						vExtents;
		_uint						iTeamIndex;
		_uint						iInteractionID;

		vector<_wstring>			szScriptTags{};
	}NPC_DATA_DESC;

	// LINK_ATTACK -> 그로기 상태에서 들어가는값
	// EXECUTION_ATTACK -> 처형 상태에서 들어가는값
	enum class NAYITBA_EXECUTION_TYPE { LINK_ATTACK, PHASE2_LINKATTACK, EXECUTION_ATTACK, END};

	typedef struct TransportDesc
	{
		_uint					iTeleportID;
		_wstring				szAreaName;

		_float3					vTransportpoint;
		_bool					bIsEnableMove;
		_uint					eTargetLevel;
	} TRANSPORT_DESC;

	enum class NOTE_TYPE { HIT, OVERLAP, END };
	typedef struct Note_Data_Desc
	{
		NOTE_TYPE				NoteType;
		DIRECTION				eDirection;

		_float					fSpawnRatio;
		_float2					vBoundAnimFrame;
	}NOTE_DATA_DESC;

	typedef struct MotionTimeDesc
	{
		float		BeginTime;
		float		EndTime;
		float		fRange;
		float		fSpeed;
		float		fAnimationSpeed;

		bool		bIsEnter;
		bool		bIsTarget;

		DIRECTION	eDirection;
	}MOTION_TIME_DESC;

	typedef struct tagColorChange
	{
		_float4 vColor;
		_float  fIntensity;
	} COLORCHANGE_DEFERRED_DESC;

	typedef struct tagShopDataDesc
	{
		_uint iID;
		_uint iPrice;
		_wstring szItemTag;
		_wstring szScript;
	}SHOP_DESC;

	typedef	struct BeatSaberCharacterDesc
	{
		long long	iScore;

		_int		iGameLife;
		_int		iMaxGameLife;

		_uint		iComboCnt;
		_float		fCharacterSpeed;
		// x : 캐릭터 최소 스피드
		// y : 캐릭터 기본 스피드
		// z : 캐릭터 최대 스피드
		_float3		fBoundCharacterSpeed;

		DIRECTION	ePreDirection;
		DIRECTION	eDirection;
	}BEATSABER_CHARACTER_DESC;
}