#include "pch.h"
#include "DataManager.h"

#include "GameInstance.h"
#include "StringHelper.h"
#include "JsonParser.h"

CDataManager::CDataManager()
{
}

HRESULT CDataManager::Initalize()
{
    if (FAILED(LoadSkillData()))
        return E_FAIL;
    
    if (FAILED(LoadAnimNotifyData()))
        return E_FAIL;

    CGameInstance::GetInstance()->Add_ThreadjobList([&](void* pArg) { LoadNaytibaData(pArg); });

    return S_OK;
}

const CHARACTER_SKILL_DESC* CDataManager::Find_SkillData(_uint iSkillID)
{
    auto iter = m_pSkillDatas.find(iSkillID);
    if (iter == m_pSkillDatas.end())
        return nullptr;

    return &iter->second;
}

const NAYTIBA_NETWORK_DESC* CDataManager::Find_NaytibaData(_uint iID)
{
    auto iter = m_pNaytibaDatas.find(iID);
    if (iter == m_pNaytibaDatas.end())
        return nullptr;

    return &iter->second;
}

const vector<ANIM_NOTIFY>* CDataManager::Find_AnimationNotifyData(const _wstring& szAnimationTag)
{
    auto iter = m_AnimationNotifyDatas.find(szAnimationTag);
    if (iter == m_AnimationNotifyDatas.end())
        return nullptr;

    return &iter->second;
}


HRESULT CDataManager::LoadNaytibaData(void* pArg)
{
    THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
    vector<string> BossDataList; 
    BossDataList.reserve(1000);

    CStringHelper::CSVRead("../Bin/DataFiles/NaytibaData/NaytibaData.csv", BossDataList);


    size_t iMaxSize = BossDataList.size();

    for (auto i = 16; i < iMaxSize;)
    {
        NAYTIBA_NETWORK_DESC BossDesc = {};
        _uint iBossID = atoi(BossDataList[i++].c_str());
        BossDesc.iNumPhase = atoi(BossDataList[i++].c_str());
       
        strcpy_s(BossDesc.szAnimationName, BossDataList[i++].c_str());
        strcpy_s(BossDesc.szMonsterName, BossDataList[i++].c_str());
        
        BossDesc.eNaytiba_Type = NAYTIBA_TYPE(atoi(BossDataList[i++].c_str()));
        BossDesc.eAI_Type = AI_TYPE(atoi(BossDataList[i++].c_str()));

        strcpy_s(BossDesc.szModelPrototype, BossDataList[i++].c_str());
        strcpy_s(BossDesc.szAIControllerPrototype, BossDataList[i++].c_str());
        strcpy_s(BossDesc.szAIBehaviorPrototype, BossDataList[i++].c_str());

        BossDesc.iMaxHealth = atoi(BossDataList[i++].c_str());
        BossDesc.iMaxShield = atoi(BossDataList[i++].c_str());

        BossDesc.fAttackCoolTime = atoi(BossDataList[i++].c_str());
        BossDesc.fAttackRange = atoi(BossDataList[i++].c_str());

        BossDesc.fColliderExtents.x = (_float)atof(BossDataList[i++].c_str());
        BossDesc.fColliderExtents.y = (_float)atof(BossDataList[i++].c_str());
        BossDesc.fColliderExtents.z = (_float)atof(BossDataList[i++].c_str());

        _uint iNumSkill = atoi(BossDataList[i++].c_str());
        for (_uint j = 0; j < iNumSkill; ++j)
            BossDesc.iAttackList.push_back(atoi(BossDataList[i++].c_str()));

        m_pNaytibaDatas.emplace(iBossID, BossDesc);
    }
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

HRESULT CDataManager::LoadAnimNotifyData(void* pArg)
{
    //	- 모든 애니메이션 관련 이벤트를 담당하는 ANIM_NOTIFY
    //  - 를 담고 있는 vector<ANIM_NOTIFY>
    //  - 들을 애니메이션 태그(_char*)로 구분짓는 map

    Json jAnim;

    wstring szFilePath{};
    szFilePath = TEXT("../Bin/DataFiles/Animation/AnimationNotifyData.json");

    _char szPath[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(szFilePath.c_str(), szPath);

    CJsonParser::ReadJsonData(szPath, jAnim);

    for (auto& pAnim : jAnim)
    {
        WCHAR szText[MAX_PATH];
        _wstring szAnimTag;

        vector<ANIM_NOTIFY> AnimationNotifyList;
        CStringHelper::ConvertUTFToWide(pAnim["szAnimationTag"].get<string>().c_str(), szText);
        szAnimTag = szText;

        for (auto& pAnimNotify : pAnim["AnimationNotifies"])
        {
            ANIM_NOTIFY AnimNotify = {};
            AnimNotify.szNotifyTag = pAnimNotify["szNotifyTag"].get<string>();
            AnimNotify.szNotifyArg01 = pAnimNotify["szNotifyArg01"].get<string>();
            AnimNotify.szNotifyArg02 = pAnimNotify["szNotifyArg02"].get<string>();
            AnimNotify.szSocketTag = pAnimNotify["szSocketTag"].get<string>();
            AnimNotify.bIsLocalPos = pAnimNotify["bIsLocalPos"].get<bool>();

            AnimNotify.vNotifyPosition = {
                pAnimNotify["vNotifyPosition"][0].get<_float>(),
                pAnimNotify["vNotifyPosition"][1].get<_float>(),
                pAnimNotify["vNotifyPosition"][2].get<_float>()
            };
            AnimNotify.vNotifyRotation = {
                pAnimNotify["vNotifyRotation"][0].get<_float>(),
                pAnimNotify["vNotifyRotation"][1].get<_float>(),
                pAnimNotify["vNotifyRotation"][2].get<_float>()
            };
            AnimNotify.iNotifyKeyFrame = pAnimNotify["iNotifyKeyFrame"].get<_uint>();
            AnimationNotifyList.push_back(AnimNotify);
        }

        m_AnimationNotifyDatas.emplace(szAnimTag, AnimationNotifyList);
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
