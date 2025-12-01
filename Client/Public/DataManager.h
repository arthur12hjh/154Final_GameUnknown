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
	const NAYTIBA_NETWORK_DESC*				Find_NaytibaData(_uint iID);
	
	const BETA_SKILL_DESC*					Find_BetaSkillData(_uint iSkillID);
	map<_uint, BETA_SKILL_DESC>*			Get_AllBetaSkillDesc();

	const vector<ANIM_NOTIFY>*				Find_AnimationNotifyData(const _wstring& szAnimationTag);

private:
	// 캐릭터 구조체는 이거 하나만있으도 될거같아서 픽스
	// map<_uint, CHARACTER_NETWORK_DESC>			m_pTextures = {};
	
	//스킬 데이터
	map<_uint, CHARACTER_SKILL_DESC>		m_pSkillDatas = {};

	//플레이어 데이터
	map<_uint, BETA_SKILL_DESC>				m_pBetaSkills = {};
	//보스 데이터
	map<_uint, NAYTIBA_NETWORK_DESC>		m_pNaytibaDatas = {};

	// 애니메이션 노티파이(이벤트) 데이터
	unordered_map<_wstring, vector<ANIM_NOTIFY>>		m_AnimationNotifyDatas = {};


private:
	HRESULT									LoadNaytibaData(void* pArg);
	HRESULT									LoadSkillData();
	HRESULT									LoadAnimNotifyData(void* pArg = nullptr);
	HRESULT									AddBetaSkill(_uint iSkillID, CHARACTER_SKILL_DESC& Desc);
public:
	static CDataManager*					Create();
	virtual void							Free() override;

};
NS_END