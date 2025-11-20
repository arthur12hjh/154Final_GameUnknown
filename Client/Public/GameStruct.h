#pragma once
#include "Engine_Defines.h"
#include <vector>

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
	// 인게임에서 바뀔거 요거
	typedef struct Player_Desc
	{
		long long		iCurrentHealth;
		long long		iCurrentShield;
		long long		iCurrentBetaEnergy;

		long long		iCurrentAttackPoint;
		long long		iCurrentShieldATK;

		float			fCurrentCTPercent;
		float			fCurrentCTDamage;
		float			fCurrentLinkApplyDamage;
	}PLAYER_DESC;

	// 스킬 구조체
	enum class SKILL_TYPE { PARRYABLE, END };
	typedef struct Character_Skill_Desc
	{
		unsigned int	iSkillID;

		char			szAnimationName[256];
		long long		iSkillDamage;

		DIRECTION		eDirection;
		SKILL_TYPE		eSkillType;
	}CHARACTER_SKILL_DESC;


	// 보스몬스터 구조체
	// 인게임용
	typedef struct Boss_NetWork_Desc
	{
		unsigned int	iNumPhase;

		char			szBossName[256];
		long long		iMaxHealth;
		long long		iMaxShield;

		//여기서 사용하는 스킬 정보
		std::vector<_uint>		iAttackList;
	}BOSS_NETWORK_DESC;

	// 인게임용
	typedef struct Boss_Desc
	{
		unsigned int	iCurrentPhase;

		long long		iCurrentHealth;
		long long		iCurrentShield;

		std::vector<CHARACTER_SKILL_DESC>	iAttackList;

	}BOSS_DESC;

	// 만약에 공격 타입같은거도 나눌거면 여기서 나눠서 사용하세요

	enum class QUEST_STATE { LOCKED, AVAILABLE, ACTIVE, COMPELETED, END };
	typedef struct QuestStruct
	{
		// 기본구조체 대충 여기서 퀘스트 타입도 만들려면 만들기
		QUEST_STATE			QuestState;
		_uint				iQuestID;

		vector<_uint>		RewardLists;
	}QUEST_STRUCT;
}