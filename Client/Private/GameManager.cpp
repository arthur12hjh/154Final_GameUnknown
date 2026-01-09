#include "pch.h"
#include "GameManager.h"
#include "GameInstance.h"

#include "DataManager.h"
#include "QuestManager.h"
#include "LockonManager.h"
#include "ShaderManager.h"
#include "PoolingManager.h"
#include "Cinematic_Manager.h"
#include "Interaction_Manager.h"

#include "Player.h"
#include "ReserveDeferred.h"
#include "BeatSaberCharacter.h"

IMPLEMENT_SINGLETON(CGameManager);

HRESULT CGameManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (FAILED(Setting_Manager(pDevice, pContext)))
        return E_FAIL;

    m_pLockonManager = CLockonManager::Create();
    if (nullptr == m_pLockonManager)
        return E_FAIL;

    m_pDataManager = CDataManager::Create();
    if (nullptr == m_pDataManager)
        return E_FAIL;

    m_pQuestManager = CQuestManager::Create();
    if (nullptr == m_pQuestManager)
        return E_FAIL;

    m_pShaderManager = CShaderManager::Create();
    if (nullptr == m_pShaderManager)
        return E_FAIL;

    m_pPoolingManager = CPoolingManager::Create(pDevice, pContext);
    if (nullptr == m_pPoolingManager)
        return E_FAIL;

    m_pCinematicManager = CCinematicManager::Create();
    if (nullptr == m_pCinematicManager)
        return E_FAIL;

    return S_OK;
}

void CGameManager::Update(_float fTimeDelta)
{
    if(nullptr != m_pShaderManager)
        m_pShaderManager->Update(fTimeDelta);

    if(nullptr != m_pCinematicManager)
        m_pCinematicManager->Update(fTimeDelta);
}

void CGameManager::Bind_GameCharacter(CPlayer* pCharacter)
{
    if (m_pPlayer == pCharacter)
        return;

    m_pPlayer = pCharacter;
    m_pLockonManager->Bind_Player(m_pPlayer);
}

void CGameManager::Bind_BeatSaberCharacter(CBeatSaberCharacter* pCharacter)
{
    if (m_pBeatSaberCharacter == pCharacter)
        return;

    m_pBeatSaberCharacter = pCharacter;
}

CPlayer* CGameManager::GetGameCharacter()
{
    Safe_AddRef(m_pPlayer);
    return m_pPlayer;
}

CBeatSaberCharacter* CGameManager::GetBeatSaberCharacter()
{
    Safe_AddRef(m_pBeatSaberCharacter);
    return m_pBeatSaberCharacter;
}

// 레벨 전환할때 할거
void CGameManager::SavePlayerDesc()
{
    const PLAYER_DESC* SavePlayerDesc = m_pPlayer->Get_Desc();
         
    Save_Level_PlayerData Desc = {};
    Desc.PlayerData = *SavePlayerDesc;

    m_pSavePlayerDesc = make_pair(Desc, true);
    m_pSavePlayerDesc.first.PlayerData.pPlayerTransform = nullptr;
    m_pSavePlayerDesc.first.PlayerData.pPlayerController = nullptr;
    m_pSavePlayerDesc.first.PlayerData.pLinkAttackTarget = nullptr;
    m_pSavePlayerDesc.first.PlayerData.pGrabBone = nullptr;
    m_pSavePlayerDesc.first.PlayerData.pGrabAttackter = nullptr;
}

void CGameManager::SetPlayerNextLevelSpawnPosition(_uint iLevelID, _uint iLevelTransportIndex)
{
    auto TransportData = m_pDataManager->Find_TransportData(iLevelTransportIndex);
    if (nullptr == TransportData)
        return;

    if (iLevelID != TransportData->eTargetLevel)
        return;

    m_pSavePlayerDesc.first.vOldPosition = TransportData->vTransportpoint;
}

void CGameManager::SetMiniGameReward(_uint iItemID, _uint iCount)
{
    // 여기서 아이템을 반환한다.
    m_MiniGameReward.iItemList.push_back(make_pair(iItemID, iCount));
}

MINIGAME_REWARD* CGameManager::GetMiniGameReward()
{
    return &m_MiniGameReward;
}

_bool CGameManager::LoadPlayerDesc(SAVE_LEVEL_PLAYERDATA& PlayerDesc)
{
    if (m_pSavePlayerDesc.second)
    {
        PlayerDesc = m_pSavePlayerDesc.first;
        return  true;
    }

    return false;
}

PLAYER_DESC* CGameManager::Get_PlayerDesc()
{
    return m_pPlayer->Get_Desc();
}

_bool CGameManager::Is_NearCharacter(_vector vPos, _float vRange)
{
    _vector vPlayerPos = m_pPlayer->GetTransform()->Get_State(STATE::POSITION);
    _float fDistance = XMVectorGetX(XMVector3Length(vPlayerPos - vPos));
    if (fDistance <= vRange)
        return true;

    return false;
}

#pragma region DataManager
const NPC_DATA_DESC* CGameManager::Find_NpcData(_uint iSkillID)
{
    return m_pDataManager->Find_NpcData(iSkillID);
}
const CHARACTER_SKILL_DESC* CGameManager::Find_SkillData(_uint iSkillID)
{
    return m_pDataManager->Find_SkillData(iSkillID);
}

const NAYTIBA_NETWORK_DESC* CGameManager::Find_BossData(_uint iBossID)
{
    return m_pDataManager->Find_NaytibaData(iBossID);
}

const BETA_SKILL_DESC* CGameManager::Find_BetaSkillData(_uint iSkillID)
{
    return m_pDataManager->Find_BetaSkillData(iSkillID);
}

map<_uint, BETA_SKILL_DESC>* CGameManager::Get_AllBetaSkillDesc()
{
    return m_pDataManager->Get_AllBetaSkillDesc();
}

const vector<ANIM_NOTIFY>* CGameManager::Find_AnimationNotifyData(const _wstring& szAnimationTag)
{
    return m_pDataManager->Find_AnimationNotifyData(szAnimationTag);
}

const INTERACTION_DATA* CGameManager::Find_InteractionData(_uint iID)
{
    return m_pDataManager->Get_InteractionData(iID);
}

const CAMERA_ANIMATION_DATA* CGameManager::Find_CameraAnimationData(_uint iCameraAnimationData)
{
    return m_pDataManager->Find_CameraAnimationData(iCameraAnimationData);
}

const SCRIPT_DESC* CGameManager::Get_ScriptData(const _wstring& szScriptTag)
{
    return m_pDataManager->Get_ScriptData(szScriptTag);
}

const CINEMATIC_DESC* CGameManager::Find_CinematicData(_uint iCinematicIndex)
{
    return m_pDataManager->Find_CinematicData(iCinematicIndex);
}

void CGameManager::EnableTransport(_uint iAreaID)
{
    m_pDataManager->EnableTransport(iAreaID);
}

const TRANSPORT_DESC* CGameManager::Find_TransportData(_uint iAreaID)
{
    return  m_pDataManager->Find_TransportData(iAreaID);
}

const map<_uint, TRANSPORT_DESC>* CGameManager::Find_AllTransportDatas()
{
    return  m_pDataManager->Find_AllTransportDatas();
}

const vector<SHOP_DESC>* CGameManager::Get_ShopDatas()
{
    return m_pDataManager->Get_ShopDatas();
}

map<_uint, CAMERA_ANIMATION_DATA>* CGameManager::Get_CameraAnimationMap()
{
    return m_pDataManager->Get_CameraAnimationMap();
}


map<_uint, CINEMATIC_DESC>* CGameManager::Get_CinematicDataMap()
{
    return m_pDataManager->Get_CinematicDataMap();
}

#ifdef _DEBUG

void CGameManager::Refresh()
{
    m_pDataManager->Refresh();
}

void CGameManager::Save_CameraAnimationData()
{
    m_pDataManager->Save_CameraAnimationData();
}

void CGameManager::Save_CinematicData()
{
    m_pDataManager->Save_CinematicData();
}

#endif

#pragma endregion

#pragma region Quest Manager
CQuest* CGameManager::Find_Quest(_uint iQuestID)
{
    return m_pQuestManager->Find_Quest(iQuestID);
}

HRESULT CGameManager::Ready_Quest(const WCHAR* szFilePath)
{
    return m_pQuestManager->Ready_Quest(szFilePath);
}

_bool CGameManager::Accept_Quest(_uint iQuestID)
{
    return m_pQuestManager->Accept_Quest(iQuestID);
}

void CGameManager::CompletedQuest(_uint iQuestID)
{
    m_pQuestManager->CompletedQuest(iQuestID);
}

HRESULT CGameManager::Add_Shader(LEVEL eLevelID, const _wstring& strShaderTag, CShader* pShader)
{
    return m_pShaderManager->Add_Shader(eLevelID, strShaderTag, pShader);
}

CShader* CGameManager::Get_Shader(LEVEL eLevelID, const _wstring& strShaderTag)
{
    return m_pShaderManager->Get_Shader(eLevelID, strShaderTag);
}

HRESULT CGameManager::Bind_CamInfo(LEVEL eLevelID)
{
    return m_pShaderManager->Bind_CamInfo(eLevelID);
}

HRESULT CGameManager::Add_ReserveDeferred(const _wstring& strReserveDeferredTag, CReserveDeferred* pReserveDeferred)
{
    return m_pShaderManager->Add_ReserveDeferred(strReserveDeferredTag, pReserveDeferred);
}

void CGameManager::Set_Active_ReserveDeferred(const _wstring& strReserveDeferredTag, _bool bFlag)
{
    m_pShaderManager->Set_Active_ReserveDeferred(strReserveDeferredTag, bFlag);
}

void CGameManager::Set_Desc_ReserveDeferred(const _wstring& strReserveDeferredTag, void* pArg)
{
    m_pShaderManager->Set_Desc_ReserveDeferred(strReserveDeferredTag, pArg);
}

void CGameManager::Change_ShaderSetting(LEVEL eLevelID, _uint iIndex)
{
    m_pShaderManager->Change_ShaderSetting(eLevelID, iIndex);
}

void CGameManager::Set_CinematicLights(_uint iFlag)
{
    m_pShaderManager->Set_CinematicLights(iFlag);
}

void CGameManager::Set_ScarletPatternFog(_bool bFlag, _float fLerpTime)
{
    m_pShaderManager->Set_ScarletPatternFog(bFlag, fLerpTime);
}

#pragma region LOCKON

CNaytiba* CGameManager::Get_LockonTarget()
{
    return m_pLockonManager->Get_LockonTarget();
}

CTransform* CGameManager::Get_TargetTransform()
{
    return m_pLockonManager->Get_TargetTransform();
}
void CGameManager::Lockon(_float fTimeDelta)
{
    m_pLockonManager->Lockon(fTimeDelta);
}
_vector CGameManager::Get_LockOnPoint()
{
    return m_pLockonManager->Get_LockOnPoint();
}
void CGameManager::Start_Lockon()
{
    m_pLockonManager->Start_Lockon();
}
_float CGameManager::Get_CurMinDist()
{
    return m_pLockonManager->Get_CurMinDist();
}
_bool CGameManager::Get_Lockon()
{
    return m_pLockonManager->Get_LockOn();
}

void CGameManager::Force_Lockon(_float fTimeDelta)
{
    m_pLockonManager->Force_Lockon(fTimeDelta);
}

void CGameManager::Force_LockOff()
{
    m_pLockonManager->Force_LockOff();
}

#pragma endregion

#pragma region Pool Manager
HRESULT CGameManager::Setting_PoolManager(_uint iLevelID)
{
    return m_pPoolingManager->Setting_PoolManager(iLevelID);
}

HRESULT CGameManager::ADD_PoolManager(_uint iLevelID, _uint iProtoTypeLevel, const WCHAR* ProtoTypeName, void* pArg, const WCHAR* szPoolTag, _uint iCount)
{
    return m_pPoolingManager->ADD_PoolManager(iLevelID, iProtoTypeLevel, ProtoTypeName, pArg, szPoolTag, iCount);
}

CGameObject* CGameManager::SetActivePoolObject(_uint iLevel, _uint iProtoTypeLevel, const WCHAR* pLayerName, const WCHAR* szPoolTag)
{
    return m_pPoolingManager->SetActivePoolObject(iLevel, iProtoTypeLevel, pLayerName, szPoolTag);
}

void CGameManager::UnActivePoolObject(_uint iLevelID, const WCHAR* szPoolTag, CGameObject* pObject)
{
    m_pPoolingManager->UnActivePoolObject(iLevelID, szPoolTag, pObject);
}
#pragma endregion


#pragma region Damage Logic
_bool CGameManager::ComputeDamageLogic(Default_Status* pInfo, const long long& iDamage, _float fPercent)
{
    long long AttackDamage = iDamage * (1.f - fPercent);
    long long GuardDamage = iDamage * fPercent;
    long long OverDamage = GuardDamage - pInfo->iCurrentShield;

    if (0 < OverDamage)
    {
        AttackDamage += OverDamage;
        pInfo->iCurrentShield = 0.f;
    }
    else
        pInfo->iCurrentShield -= GuardDamage;

    pInfo->iCurrentHealth -= AttackDamage;
    if (0 >= pInfo->iCurrentHealth)
        pInfo->iCurrentHealth = 0.f;

    return true;
}
HRESULT CGameManager::Play_Cinematic(_uint iCinematicID, function<void()> FinishedFunc)
{
    return m_pCinematicManager->Play_Cinematic(iCinematicID, FinishedFunc);
}
HRESULT CGameManager::Load_Level_CinematicObjectData(const _char* szFilePath)
{
    return m_pCinematicManager->Load_Level_CinematicObjectData(szFilePath);
}
_bool CGameManager::Is_CinematicPlaying()
{
    return m_pCinematicManager->Is_CinematicPlaying();
}
HRESULT CGameManager::Skip_Cinematic()
{
    return m_pCinematicManager->Skip_Cinematic();
}
map<_wstring, CCinematicObject*>* CGameManager::Get_CinematicObjectsMap()
{
    return m_pCinematicManager->Get_CinematicObjectsMap();
}
#pragma endregion

void CGameManager::Play_BossBGM(_uint iBossID, _uint iBossPhase)
{
    m_FieldBgmInfo.iBossID = iBossID;
    m_FieldBgmInfo.iBossPhase = iBossPhase;
    m_FieldBgmInfo.eBGMState = BGM_SOUND_STATE::INTRO;

    m_pGameInstance->Manager_StopSound(CHANNELID::BGM);
    if (8 == iBossID)
    {
        if (1 == iBossPhase)
        {
            m_pGameInstance->Manager_PlayBGM(TEXT("BGM_BOSS_SCARLET_P1_INTRO.wav"), 3.f, 1.f, [&](FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2)
                {
                    BGM_FinishedSoundCallBack(channelcontrol, controltype, callbacktype, commanddata1, commanddata2);
                }
            );
          
        }
        else if (2 == iBossPhase)
        {
            m_pGameInstance->Manager_PlayBGM(TEXT("BGM_BOSS_SCARLET_P2_INTRO.wav"), 3.f, 1.f, [&](FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2)
                {
                    BGM_FinishedSoundCallBack(channelcontrol, controltype, callbacktype, commanddata1, commanddata2);
                }
            );
        }
    }
    else if (1 == iBossID)
    {
        if (1 == iBossPhase)
        {
            m_pGameInstance->Manager_PlayBGM(TEXT("BGM_XION_BOSS_RAVENBEAST_P1_INTRO.wav"), 3.f, 1.f, [&](FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2)
                {
                    BGM_FinishedSoundCallBack(channelcontrol, controltype, callbacktype, commanddata1, commanddata2);
                }
            );

        }
    }
}

void CGameManager::Release_GameMgr()
{
    Safe_Release(m_pPoolingManager);
    Safe_Release(m_pCinematicManager);
}

void CGameManager::BGM_FinishedSoundCallBack(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void* commanddata1, void* commanddata2)
{
    m_pGameInstance->Manager_StopSound(CHANNELID::BGM);
    if (BGM_SOUND_STATE::INTRO == m_FieldBgmInfo.eBGMState)
    {
        if (8 == m_FieldBgmInfo.iBossID)
        {
            if (1 == m_FieldBgmInfo.iBossPhase)
                m_pGameInstance->Manager_PlayBGM(TEXT("BGM_BOSS_SCARLET_P1_LOOP.wav"), 1.f, 1.f);
            else if (2 == m_FieldBgmInfo.iBossPhase)
                m_pGameInstance->Manager_PlayBGM(TEXT("BGM_BOSS_SCARLET_P2_LOOP.wav"), 1.f, 1.f);
        }

        if (1 == m_FieldBgmInfo.iBossID)
        {
            if (1 == m_FieldBgmInfo.iBossPhase)
                m_pGameInstance->Manager_PlayBGM(TEXT("BGM_XION_BOSS_RAVENBEAST_P1_LOOP.wav"), 1.f, 1.f);
        }
    }
}

HRESULT CGameManager::Setting_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;
    m_pGameInstance = CGameInstance::GetInstance();

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);

    return S_OK;
}

void CGameManager::Free()
{
    __super::Free();

    //Safe_Release(m_pPlayer);
    Safe_Release(m_pDataManager);
    Safe_Release(m_pQuestManager);
    Safe_Release(m_pLockonManager);
    Safe_Release(m_pShaderManager);
    //Safe_Release(m_pPoolingManager);
    //Safe_Release(m_pCinematicManager);
    
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
