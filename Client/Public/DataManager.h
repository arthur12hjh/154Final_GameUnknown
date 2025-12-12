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

	const INTERACTION_DATA*					Get_InteractionData(_uint iID);

	const vector<ANIM_NOTIFY>*				Find_AnimationNotifyData(const _wstring& szAnimationTag);

	const CAMERA_ANIMATION_DATA*			Find_CameraAnimationData(_uint iCameraAnimationData);

	void									Save_CameraAnimationData();


#ifdef _DEBUG
	map<_uint, CAMERA_ANIMATION_DATA>* Get_CameraAnimationMap();
#endif

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

	// 상호작용 오브젝트 데이터
	map<_uint, INTERACTION_DATA>			m_pInteractionDatas = {};

	// 카메라 애니메이션 전용 데이터
	map<_uint, CAMERA_ANIMATION_DATA>		m_CameraAnimationDatas = {};


private:
	HRESULT									LoadNaytibaData(void* pArg);
	HRESULT									LoadInteractionData(void* pArg);
	HRESULT									LoadSkillData();
	HRESULT									LoadAnimNotifyData(void* pArg = nullptr);
	HRESULT									LoadCameraAnimationData(void* pArg = nullptr);
	HRESULT									AddBetaSkill(_uint iSkillID, CHARACTER_SKILL_DESC& Desc);
public:
	static CDataManager*					Create();
	virtual void							Free() override;

};
NS_END