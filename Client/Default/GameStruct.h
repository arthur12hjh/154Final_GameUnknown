#pragma once

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
	typedef struct Character_Desc
	{
		long long		iCurrentHealth;
		long long		iCurrentShield;
		long long		iCurrentBetaEnergy;

		long long		iCurrentAttackPoint;
		long long		iCurrentShieldATK;

		float			fCurrentCTPercent;
		float			fCurrentCTDamage;
		float			fCurrentLinkApplyDamage;
	}CHARACTER_DESC;

	// 만약에 공격 타입같은거도 나눌거면 여기서 나눠서 사용하세요

}