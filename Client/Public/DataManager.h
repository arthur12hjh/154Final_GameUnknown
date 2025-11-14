#pragma once
#include "Client_Defines.h"
#include "GameStruct.h"
#include "Base.h"


NS_BEGIN(Client)
class CDataManager final : public CBase
{
private:
	CDataManager();
	virtual ~CDataManager() = default;

	HRESULT									Initalize();

public :
	const CHARACTER_SKILL_DESC*				Find_SkillData(_uint iSkillID);
	const BOSS_NETWORK_DESC*				Find_BossData(_uint iBossID);

private:
	// 캐릭터 구조체는 이거 하나만있으도 될거같아서 픽스
	// map<_uint, CHARACTER_NETWORK_DESC>			m_pTextures = {};
	
	//스킬 데이터
	map<_uint, CHARACTER_SKILL_DESC>		m_pSkillDatas = {};

	//보스 데이터
	map<_uint, BOSS_NETWORK_DESC>			m_pBossDatas = {};

private:
	HRESULT									LoadBossData(void* pArg);
	HRESULT									LoadSkillData();

public:
	static CDataManager*					Create();
	virtual void							Free() override;

};
NS_END