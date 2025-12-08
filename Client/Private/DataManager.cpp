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

const BETA_SKILL_DESC* CDataManager::Find_BetaSkillData(_uint iSkillID)
{
    auto iter = m_pBetaSkills.find(iSkillID);
    if (iter == m_pBetaSkills.end())
        return nullptr;

    return &iter->second;
}

map<_uint, BETA_SKILL_DESC>* CDataManager::Get_AllBetaSkillDesc()
{
    return &m_pBetaSkills;
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
    for (auto i = 18; i < iMaxSize;)
    {
        NAYTIBA_NETWORK_DESC BossDesc = {};
        BossDesc.iMonsetID = atoi(BossDataList[i++].c_str());
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
        BossDesc.iMaxStamina = atoi(BossDataList[i++].c_str());
        BossDesc.fMoveSpeed = atoi(BossDataList[i++].c_str());

        BossDesc.fAttackCoolTime = atoi(BossDataList[i++].c_str());
        BossDesc.fAttackRange = atoi(BossDataList[i++].c_str());

        BossDesc.fColliderExtents.x = (_float)atof(BossDataList[i++].c_str());
        BossDesc.fColliderExtents.y = (_float)atof(BossDataList[i++].c_str());
        BossDesc.fColliderExtents.z = (_float)atof(BossDataList[i++].c_str());

        _uint iNumSkill = atoi(BossDataList[i++].c_str());
        for (_uint j = 0; j < iNumSkill; ++j)
            BossDesc.iAttackList.push_back(atoi(BossDataList[i++].c_str()));

        m_pNaytibaDatas.emplace(BossDesc.iMonsetID, BossDesc);
    }
    return S_OK;
}

HRESULT CDataManager::LoadSkillData()
{
    vector<string> SkillDataList;
    SkillDataList.reserve(1000);

    CStringHelper::CSVRead("../Bin/DataFiles/SkillData/SkillData.csv", SkillDataList);
    size_t iMaxSize = SkillDataList.size();

    for (auto i = 11; i < iMaxSize;)
    {
        CHARACTER_SKILL_DESC SkillDesc = {};
        SkillDesc.iSkillID = atoi(SkillDataList[i++].c_str());
        strcpy_s(SkillDesc.szAnimationName, SkillDataList[i++].c_str());
        strcpy_s(SkillDesc.szHitAnimationName, SkillDataList[i++].c_str());
        strcpy_s(SkillDesc.szLinkBoneName, SkillDataList[i++].c_str());

        SkillDesc.iSkillDamage = atoi(SkillDataList[i++].c_str());
        SkillDesc.fRange = atof(SkillDataList[i++].c_str());
        SkillDesc.vHitBoxExtents.x = atof(SkillDataList[i++].c_str());
        SkillDesc.vHitBoxExtents.y = atof(SkillDataList[i++].c_str());
        SkillDesc.vHitBoxExtents.z = atof(SkillDataList[i++].c_str());
        SkillDesc.iMaxComboCount = atoi(SkillDataList[i++].c_str());

        SkillDesc.eATK_Direction = ATTACK_DIRECTION(atoi(SkillDataList[i++].c_str()));
        SkillDesc.eSkillType = SKILL_TYPE(atoi(SkillDataList[i++].c_str()));
        SkillDesc.eProPerty = SKILL_PROPERTY(atoi(SkillDataList[i++].c_str()));

        m_pSkillDatas.emplace(SkillDesc.iSkillID, SkillDesc);

        if (SKILL_TYPE::BETA_SKILL == SkillDesc.eSkillType)
            AddBetaSkill(SkillDesc.iSkillID, SkillDesc);
    }

    return S_OK;
}

HRESULT CDataManager::LoadAnimNotifyData(void* pArg)
{
    //	- 모든 애니메이션 관련 이벤트를 담당하는 ANIM_NOTIFY
    //  - 를 담고 있는 vector<ANIM_NOTIFY>
    //  - 들을 애니메이션 태그(_char*)로 구분짓는 map

    // _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
    _finddatai64_t  fd;

    // _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
    // json도 되려나 이거..
    intptr_t handle = _findfirst64("../Bin/DataFiles/Animation/*.json*", &fd);

    if (handle == -1)
        return S_OK;

    int iResult = 0;

    _wstring szFrontPath = TEXT("../Bin/DataFiles/Animation/");

    while (iResult != -1)
    {
        int iLength = strlen(fd.name) + 1;
        WCHAR* pFileName = new WCHAR[iLength];
        ZeroMemory(pFileName, sizeof(WCHAR) * iLength);

        // 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
        MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pFileName, iLength);

        _wstring szFullPath = szFrontPath + pFileName;
        _wstring szFilePath = pFileName;


        Json jAnim;

        _char szPath[MAX_PATH]{};
        CStringHelper::ConvertWideToUTF(szFullPath.c_str(), szPath);

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
                AnimNotify.szNotifyArg03 = pAnimNotify["szNotifyArg03"].get<string>();
                AnimNotify.szNotifyArg04 = pAnimNotify["szNotifyArg04"].get<string>();
                AnimNotify.szNotifyArg05 = pAnimNotify["szNotifyArg05"].get<string>();
                AnimNotify.szNotifyArg06 = pAnimNotify["szNotifyArg06"].get<string>();
                AnimNotify.szNotifyArg07 = pAnimNotify["szNotifyArg07"].get<string>();
                AnimNotify.szNotifyArg08 = pAnimNotify["szNotifyArg08"].get<string>();

                AnimNotify.iNumData01 = pAnimNotify["iNumData01"].get<_int>();
                AnimNotify.iNumData02 = pAnimNotify["iNumData02"].get<_int>();
                AnimNotify.iNumData03 = pAnimNotify["iNumData03"].get<_int>();
                AnimNotify.iNumData04 = pAnimNotify["iNumData04"].get<_int>();
                AnimNotify.iNumData05 = pAnimNotify["iNumData05"].get<_int>();
                AnimNotify.iNumData06 = pAnimNotify["iNumData06"].get<_int>();
                AnimNotify.iNumData07 = pAnimNotify["iNumData07"].get<_int>();
                AnimNotify.iNumData08 = pAnimNotify["iNumData08"].get<_int>();

                AnimNotify.fNumData01 = pAnimNotify["fNumData01"].get<_float>();
                AnimNotify.fNumData02 = pAnimNotify["fNumData02"].get<_float>();
                AnimNotify.fNumData03 = pAnimNotify["fNumData03"].get<_float>();
                AnimNotify.fNumData04 = pAnimNotify["fNumData04"].get<_float>();

                AnimNotify.szSocketTag = pAnimNotify["szSocketTag"].get<string>();

                AnimNotify.vNotifyScale = {
                   pAnimNotify["vNotifyScale"][0].get<_float>(),
                   pAnimNotify["vNotifyScale"][1].get<_float>(),
                   pAnimNotify["vNotifyScale"][2].get<_float>()
                };

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


        //_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
        iResult = _findnext64(handle, &fd);
        Safe_Delete_Array(pFileName);
    }



    return S_OK;
}

HRESULT CDataManager::AddBetaSkill(_uint iSkillID, CHARACTER_SKILL_DESC& Desc)
{
    BETA_SKILL_DESC BetaSkillDesc = {};

    memcpy(&BetaSkillDesc, &Desc, sizeof(CHARACTER_SKILL_DESC));

    switch(iSkillID)
    {
    //Charge Slash
    case 1004:
        BetaSkillDesc.iRequiredBetaGauge = 4;
        break;
    //테스트 용으로 0 처리
    case 1005:
        BetaSkillDesc.iRequiredBetaGauge = 0;
        break;
    default:
        BetaSkillDesc.iRequiredBetaGauge = 10;
        break;
    }

    m_pBetaSkills.emplace(iSkillID, BetaSkillDesc);

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
