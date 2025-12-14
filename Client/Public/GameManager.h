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
class CShaderManager;
class CPoolingManager;

class CGameManager final : public CBase
{
	DECLARE_SINGLETON(CGameManager);

private:
	CGameManager() = default;
	virtual ~CGameManager() = default;

public :
	HRESULT						Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void						Update(_float fTimeDelta);
#pragma region Default 
	void						Bind_GameCharacter(class CPlayer* pCharacter);
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
	const CAMERA_ANIMATION_DATA*	Find_CameraAnimationData(_uint iCameraAnimationData);

#ifdef _DEBUG
	map<_uint, CAMERA_ANIMATION_DATA>* Get_CameraAnimationMap();
	void							Save_CameraAnimationData();
#endif
#pragma endregion

#pragma region Quest Manager
	class CQuest*				Find_Quest(_uint iQuestID);

	HRESULT						Ready_Quest(const WCHAR* szFilePath);
	_bool						Accept_Quest(_uint iQuestID);
	void						CompletedQuest(_uint iQuestID);
#pragma endregion

#pragma region SHADER_MANAGER
	HRESULT		Add_Shader(LEVEL eLevelID, const _wstring& strShaderTag, class CShader* pShader);
	class CShader* Get_Shader(LEVEL eLevelID, const _wstring& strShaderTag);
	HRESULT Bind_CamInfo(LEVEL eLevelID);
#pragma endregion

#pragma region LOCKON
public:
	CTransform* Get_TargetTransform();
	void		Lockon(_float fTimeDelta);

	void		Start_Lockon();
	_float		Get_CurMinDist();
	_bool		Get_Lockon();

#pragma endregion

#pragma region Pool Manager
	HRESULT											Setting_PoolManager(_uint iLevelID);
	HRESULT											ADD_PoolManager(_uint iLevelID, _uint iProtoTypeLevel, const WCHAR* ProtoTypeName, void* pArg, const WCHAR* szPoolTag, _uint iCount);
	CGameObject*									SetActivePoolObject(_uint iLevel, _uint iProtoTypeLevel, const WCHAR* pLayerName, const WCHAR* szPoolTag);
	void											UnActivePoolObject(_uint iLevelID, const WCHAR* szPoolTag, CGameObject* pObject);
#pragma endregion

#pragma region Damage Logic
	_bool					ComputeDamageLogic(Default_Status* pInfo, const long long& iDamage, _float fPercent = 0.7f);
#pragma endregion




private :
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

	CDataManager*			m_pDataManager = { nullptr };
	CQuestManager*			m_pQuestManager = { nullptr };
	CLockonManager*			m_pLockonManager = { nullptr };
	CShaderManager*			m_pShaderManager = { nullptr };
	CPoolingManager*		m_pPoolingManager = { nullptr };
	
	class CPlayer*			m_pPlayer = { nullptr };
private :
	HRESULT						Setting_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual void				Free() override;

};
NS_END