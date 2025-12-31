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

	HRESULT												Initalize();

public :
	const CHARACTER_SKILL_DESC*							Find_SkillData(_uint iSkillID);
	const NAYTIBA_NETWORK_DESC*							Find_NaytibaData(_uint iID);
	
	const BETA_SKILL_DESC*								Find_BetaSkillData(_uint iSkillID);
	const NPC_DATA_DESC*								Find_NpcData(_uint iSkillID);
	map<_uint, BETA_SKILL_DESC>*						Get_AllBetaSkillDesc();

	const INTERACTION_DATA*								Get_InteractionData(_uint iID);

	const SCRIPT_DESC*									Get_ScriptData(const _wstring& szScriptTag);

	const vector<ANIM_NOTIFY>*							Find_AnimationNotifyData(const _wstring& szAnimationTag);

	const CAMERA_ANIMATION_DATA*						Find_CameraAnimationData(_uint iCameraAnimationData);

	const CINEMATIC_DESC*								Find_CinematicData(_uint iCinematicDataID);

	void												Save_CameraAnimationData();
	void												Save_CinematicData();
	
	void												EnableTransport(_uint iAreaID);
	const vector<TRANSPORT_DESC>*						Find_TransportData();
	const TRANSPORT_DESC*								Find_TransportData(_uint iAreaID);

	map<_uint, CAMERA_ANIMATION_DATA>*					Get_CameraAnimationMap();
	map<_uint, CINEMATIC_DESC>*							Get_CinematicDataMap();

	const vector<SHOP_DESC>*							Get_ShopDatas();

	void												Refresh();

private:
	//스킬 데이터
	map<_uint, CHARACTER_SKILL_DESC>					m_pSkillDatas = {};

	//플레이어 데이터
	map<_uint, BETA_SKILL_DESC>							m_pBetaSkills = {};

	//NPC 데이터
	map<_uint, NPC_DATA_DESC>							m_pNpcDatas = {};

	//보스 데이터
	map<_uint, NAYTIBA_NETWORK_DESC>					m_pNaytibaDatas = {};

	// 애니메이션 노티파이(이벤트) 데이터
	unordered_map<_wstring, vector<ANIM_NOTIFY>>		m_AnimationNotifyDatas = {};

	// 상호작용 오브젝트 데이터
	map<_uint, INTERACTION_DATA>						m_pInteractionDatas = {};

	// 카메라 애니메이션 전용 데이터
	map<_uint, CAMERA_ANIMATION_DATA>					m_CameraAnimationDatas = {};

	// 시네마틱 전용 데이터
	map<_uint, CINEMATIC_DESC>							m_CinematicDatas = {};

	// 대화 스크립트 데이터
	unordered_map<_wstring, SCRIPT_DESC>				m_ScriptDatas = {};

	// 캐릭터 이동 위치 데이터
	vector<TRANSPORT_DESC>								m_Transports = {};

	// 상점 데이터
	vector<SHOP_DESC>									m_ShopDatas = {};

private:
	HRESULT												LoadNaytibaData(void* pArg);
	HRESULT												LoadNpcData(void* pArg);
	HRESULT												LoadInteractionData(void* pArg);
	HRESULT												LoadScriptData(void* pArg);
	HRESULT												LoadSkillData();
	HRESULT												LoadAnimNotifyData(void* pArg = nullptr);
	HRESULT												LoadCameraAnimationData(void* pArg = nullptr);
	HRESULT												LoadCinematicData(void* pArg = nullptr);
	HRESULT												LoadTransportData(void* pArg = nullptr);
	HRESULT												LoadShopData(void* pArg = nullptr);
	HRESULT												AddBetaSkill(_uint iSkillID, CHARACTER_SKILL_DESC& Desc);

	// 한글 때문에 만듦
	_wstring											UTF8ToWString(const string& str);

public:
	static CDataManager*								Create();
	virtual void										Free() override;

};
NS_END