#include "pch.h"
#include "GameManager.h"
#include "GameInstance.h"

#include "DataManager.h"
#include "QuestManager.h"
#include "LockonManager.h"
#include "Interaction_Manager.h"

#include "Player.h"

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

    return S_OK;
}

void CGameManager::Bind_GameCharacter(CPlayer* pCharacter)
{
    if (m_pPlayer == pCharacter)
        return;

    m_pPlayer = pCharacter;
    m_pLockonManager->Bind_Player(m_pPlayer);
}

CPlayer* CGameManager::GetGameCharacter()
{
    Safe_AddRef(m_pPlayer);
    return m_pPlayer;
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

#pragma region LOCKON

CTransform* CGameManager::Get_TargetTransform()
{
    return m_pLockonManager->Get_TargetTransform();
}
void CGameManager::Lockon(_float fTimeDelta)
{
    m_pLockonManager->Lockon(fTimeDelta);
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
    return m_pLockonManager->Get_Lockon();
}
#pragma endregion

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

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
