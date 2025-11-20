#include "pch.h"
#include "QuestManager.h"

#include "GameInstance.h"
#include "TestQuest.h"

CQuestManager::CQuestManager()
{
}

HRESULT CQuestManager::Initialize()
{
    if (FAILED(Ready_Quest(TEXT(""))))
        return E_FAIL;

    return S_OK;
}

CQuest* CQuestManager::Find_Quest(_uint iQuestID)
{
    auto iter = m_Quests.find(iQuestID);
    if (iter == m_Quests.end())
        return nullptr;

    return iter->second;
}

HRESULT CQuestManager::Ready_Quest(const WCHAR* szFilePath)
{
    // 여기서 대충 퀘스트 읽어서 퀘스트 하기
    // 뭐 그런 기능 있으면 좋을거같기도하고
    auto pTestQuest = CTestQuest::Create();
    m_Quests.emplace(1, pTestQuest);

    return S_OK;
}

_bool CQuestManager::Accept_Quest(_uint iQuestID)
{
    //대충 플레이어나 어딘가에 퀘스트 수주했다고 넘겨주기
    auto pQuest = Find_Quest(iQuestID);
    if (nullptr == pQuest)
        return false;
    
    pQuest->Accept_Quest();
    m_AcceptQuests.insert(iQuestID);
    return true;
}

void CQuestManager::CompletedQuest(_uint iQuestID)
{
    auto iter = m_AcceptQuests.find(iQuestID);
    if (iter == m_AcceptQuests.end())
        return;

    auto pQuest = Find_Quest(iQuestID);
    if (nullptr == pQuest)
        return;

    pQuest->Clear();
    m_CompletedQuests.insert(*iter);
    m_AcceptQuests.erase(iter);
}

CQuestManager* CQuestManager::Create()
{
    CQuestManager* pQuestManager = new CQuestManager();
    if (FAILED(pQuestManager->Initialize()))
    {
        Safe_Release(pQuestManager);
        MSG_BOX("Create Fail : Quest Manager");
    }
    return pQuestManager;
}

void CQuestManager::Free()
{
    __super::Free();

    for (auto& iter : m_Quests)
        Safe_Release(iter.second);

    m_Quests.clear();
}
