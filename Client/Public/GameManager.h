#pragma once
#include "Client_Defines.h"
#include "GameStruct.h"

#include "UIResourceStore.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CTexture;
NS_END

NS_BEGIN(Client)
class CUIResourceStore;

class CInteraction_Manager;
class CInteraction_Component;

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
	void						Bind_GameCharacter(CGameObject* pCharacter);

	// 플레이어블 캐릭터 꺼내오기
	// 캐릭터 꺼내오면 레퍼런스 증가함 
	// 다 쓰면 감소시켜주세요
	CGameObject*				GetGameCharacter();

	_bool						Is_NearCharacter(_vector vPos, _float vRange);

#pragma endregion

#pragma region UIResourceManager
	HRESULT Add_UI_Texture(_uint iProtoLevel, const _wstring& szTextureProtoTag, const _wstring& szTextureTag, const _wstring& szFilePath, _uint iTextureIndex = 0, void* pArg = nullptr);
	CUIResourceStore::UI_TEXTURE_DESC Get_UI_Texture_Desc(const WCHAR* szTextureTag);
	const unordered_map<_wstring, CUIResourceStore::UI_TEXTURE_DESC>* Get_UI_Texture_Descs();
	void Clear_UI_Texture_Descs();

	//HRESULT Add_UI_Texture(_uint iProtoLevel, const _wstring& szTextureProtoTag, const _wstring& szTextureTag, _uint iTextureIndex, void* pArg = nullptr);
	//CTexture* Get_UI_TextureCom(const WCHAR* szTextureTag);
	//_uint Get_UI_Texture_Index(const WCHAR* szTextureTag);
	//
	//const unordered_map<_wstring, CTexture*>* Get_UI_Textures();
	//const unordered_map<_wstring, _uint>* Get_UI_TextureIndices();
#pragma endregion

#pragma region DataManager
	const CHARACTER_SKILL_DESC*	Find_SkillData(_uint iSkillID);
	const BOSS_NETWORK_DESC*	Find_BossData(_uint iBossID);
#pragma endregion

#pragma region Interaction Manager
	void									Interaction_Update();
	void									SetInteractionBaseObject(CGameObject* pObject = nullptr);
	void									ADD_Interaction(CInteraction_Component* pInteraction_Com);

	CInteraction_Component*					GetNearInteraction();
	vector<CInteraction_Component*>*		GetAllInteraction();
#pragma endregion

private :
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	CGameInstance*				m_pGameInstance = nullptr;

	CGameObject*				m_pPlayer = nullptr;
	
	CUIResourceStore*			m_pUIResourceStore = nullptr;
	CInteraction_Manager*		m_pInteraction_Manager = nullptr;
	CDataManager*				m_pDataManager = nullptr;

private :
	HRESULT						Setting_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT						Ready_UIResourceStore();

public:
	virtual void				Free() override;

};
NS_END