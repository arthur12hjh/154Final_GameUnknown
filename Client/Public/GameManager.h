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
class CQuestManager;
class CDataManager;

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
	const PLAYER_DESC*			Get_PlayerDesc();
	_bool						Is_NearCharacter(_vector vPos, _float vRange);

#pragma endregion

#pragma region DataManager
	const CHARACTER_SKILL_DESC*		Find_SkillData(_uint iSkillID);
	const NAYTIBA_NETWORK_DESC*		Find_BossData(_uint iBossID);

	const vector<ANIM_NOTIFY>*		Find_AnimationNotifyData(const _wstring& szAnimationTag);
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

private :
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	CGameInstance*				m_pGameInstance = nullptr;
	CQuestManager*				m_pQuestManager = nullptr;

	CPlayer*					m_pPlayer = nullptr;
	CDataManager*				m_pDataManager = nullptr;

private :
	HRESULT						Setting_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual void				Free() override;

};
NS_END