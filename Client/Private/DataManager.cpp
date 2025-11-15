#include "pch.h"
#include "DataManager.h"

#include "GameInstance.h"
#include "StringHelper.h"

CDataManager::CDataManager()
{
}

HRESULT CDataManager::Initalize()
{
    if (FAILED(LoadSkillData()))
        return E_FAIL;

    CGameInstance::GetInstance()->Add_ThreadjobList([&](void* pArg) { this->LoadBossData(pArg); });

    return S_OK;
}

const CHARACTER_SKILL_DESC* CDataManager::Find_SkillData(_uint iSkillID)
{
    auto iter = m_pSkillDatas.find(iSkillID);
    if (iter == m_pSkillDatas.end())
        return nullptr;

    return &iter->second;
}

const BOSS_NETWORK_DESC* CDataManager::Find_BossData(_uint iBossID)
{
    auto iter = m_pBossDatas.find(iBossID);
    if (iter == m_pBossDatas.end())
        return nullptr;

    return &iter->second;
}

HRESULT CDataManager::LoadBossData(void* pArg)
{
    THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
    vector<string> BossDataList; 
    BossDataList.reserve(1000);

    CStringHelper::CSVRead("../Bin/DataFiles/BossData/Boss.csv", BossDataList);

    BOSS_NETWORK_DESC BossDesc = {};
    size_t iMaxSize = BossDataList.size();

    for (auto i = 7; i < iMaxSize; i += 7)
    {
        _uint iBossID = atoi(BossDataList[i].c_str());
        BossDesc.iNumPhase = atoi(BossDataList[i + 1].c_str());

        strcpy_s(BossDesc.szBossName, BossDataList[i + 2].c_str());
        BossDesc.iMaxHealth = atoi(BossDataList[i + 3].c_str());
        BossDesc.iMaxShield = atoi(BossDataList[i + 4].c_str());

        _uint iNumSkill = atoi(BossDataList[i + 5].c_str());
        for (_uint j = 0; j < iNumSkill; ++j)
            BossDesc.iAttackList.push_back(atoi(BossDataList[i + 5 + j].c_str()));

        m_pBossDatas.emplace(iBossID, BossDesc);
    }

    Desc->OnCompleted(this_thread::get_id());
    return S_OK;
}

HRESULT CDataManager::LoadSkillData()
{
    vector<string> SkillDataList;
    SkillDataList.reserve(1000);

    CStringHelper::CSVRead("../Bin/DataFiles/SkillData/SkillData.csv", SkillDataList);

    CHARACTER_SKILL_DESC SkillDesc = {};
    size_t iMaxSize = SkillDataList.size();

    for (auto i = 5; i < iMaxSize; i += 5)
    {
        SkillDesc.iSkillID = atoi(SkillDataList[i].c_str());
        strcpy_s(SkillDesc.szAnimationName, SkillDataList[i + 1].c_str());

        SkillDesc.iSkillDamage = atoi(SkillDataList[i + 2].c_str());
        SkillDesc.eDirection = DIRECTION(atoi(SkillDataList[i + 3].c_str()));
        SkillDesc.eSkillType = SKILL_TYPE(atoi(SkillDataList[i + 4].c_str()));

        m_pSkillDatas.emplace(SkillDesc.iSkillID, SkillDesc);
    }

    return S_OK;
}

CDataManager* CDataManager::Create()
{
    CDataManager* pDataManager = new CDataManager();
    if (FAILED(pDataManager->Initalize()))
    {
        Safe_Release(pDataManager);
        MSG_BOX("Create Fail : Data Manager");
    }
    return pDataManager;
}

void CDataManager::Free()
{
    __super::Free();
}
