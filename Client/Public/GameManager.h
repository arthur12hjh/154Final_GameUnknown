#pragma once
#include "Client_Defines.h"
#include "GameStruct.h"

#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CTexture;
class CEventHandle;
NS_END

NS_BEGIN(Client)
struct Default_Status;
class CQuestManager;
class CDataManager;
class CLockonManager;
class CPoolingManager;

class CGameManager final : public CBase
{
	DECLARE_SINGLETON(CGameManager);

private:
	CGameManager() = default;
	virtual ~CGameManager() = default;

public :
	HRESULT						Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

#pragma region Default 
	// 캐릭터 바인딩
	// 어떤 캐릭터를 플레이어가 지금 조종중인지를 바인딩
	// 이전에 플레이어 바인딩되어있던건 참조 카운트 감소하고
	// 새로운 플레이어 바인딩되면 참조카운트 증가함
	void						Bind_GameCharacter(class CPlayer* pCharacter);

	// 플레이어블 캐릭터 꺼내오기
	// 캐릭터 꺼내오면 레퍼런스 증가함 
	// 다 쓰면 감소시켜주세요
	class CPlayer*				GetGameCharacter();
	PLAYER_DESC*				Get_PlayerDesc();

	_bool						Is_NearCharacter(_vector vPos, _float vRange);

#pragma endregion

#pragma region DataManager
	const CHARACTER_SKILL_DESC*		Find_SkillData(_uint iSkillID);
	const NAYTIBA_NETWORK_DESC*		Find_BossData(_uint iBossID);
	const BETA_SKILL_DESC*			Find_BetaSkillData(_uint iSkillID);
	map<_uint, BETA_SKILL_DESC>*	Get_AllBetaSkillDesc();
	const vector<ANIM_NOTIFY>*		Find_AnimationNotifyData(const _wstring& szAnimationTag);
	const INTERACTION_DATA*			Find_InteractionData(_uint iID);
#pragma endregion

#pragma region Quest Manager
	class CQuest*				Find_Quest(_uint iQuestID);

	// 레벨에 맞는 퀘스트들 받아오거나 퀘스트 전체리스트를 들고있자.
	// 퀘스트는  천천히 로드해도됨
	HRESULT						Ready_Quest(const WCHAR* szFilePath);

	// 퀘스트를 받는다.
	// 수락한 퀘스트가 어떤 이벤트를 참조해야하는지는 매개변수로
	_bool						Accept_Quest(_uint iQuestID);

	// 완료된 퀘스트들 여기다가 보관
	void						CompletedQuest(_uint iQuestID);
#pragma endregion


#pragma region LOCKON
public:
	// 락온 모드일때 타겟의 트랜스폼 가져오는 함수.
	// 락온 모드가 아니라면 nullptr을 반환.
	CTransform* Get_TargetTransform();
	// 실질적인 락온 로직이 들어가있음. 락온 모드일때 세팅해주는 함수.
	void		Lockon(_float fTimeDelta);
	// 락온 스타트
	void		Start_Lockon();
	_float		Get_CurMinDist();
	_bool		Get_Lockon();

#pragma endregion

#pragma region Pool Manager
	HRESULT											Setting_PoolManager(_uint iLevelID);
	CGameObject*									SetActivePoolObject(_uint iLevel, const WCHAR* pLayerName, const WCHAR* szPoolTag);
	void											UnActivePoolObject(const WCHAR* szPoolTag, CGameObject* pObject);
#pragma endregion

#pragma region Damage Logic
	// 매개변수 1 : Default Status 구조체 정보
	// 매개변수 2 : 데미지 량
	// 매개변수 3 : 쉴드로 감쇠할 데미지의 퍼센트
	_bool					ComputeDamageLogic(Default_Status* pInfo, const long long& iDamage, _float fPercent = 0.7f);
#pragma endregion




private :
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

	CDataManager*			m_pDataManager = { nullptr };
	CQuestManager*			m_pQuestManager = { nullptr };
	CLockonManager*			m_pLockonManager = { nullptr };
	CPoolingManager*		m_pPoolingManager = { nullptr };

	//플레이어 매니저 굳이 안만들고 이대로 둘게요 이게 더 편할듯
	class CPlayer*			m_pPlayer = { nullptr };
private :
	HRESULT						Setting_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual void				Free() override;

};
NS_END