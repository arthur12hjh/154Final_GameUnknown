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

    if (FAILED(LoadCameraAnimationData()))
        return E_FAIL;

    if (FAILED(LoadCinematicData()))
        return E_FAIL;

    CGameInstance::GetInstance()->Add_ThreadjobList([&](void* pArg) { LoadNaytibaData(pArg); });
    CGameInstance::GetInstance()->Add_ThreadjobList([&](void* pArg) { LoadInteractionData(pArg); });

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

const INTERACTION_DATA* CDataManager::Get_InteractionData(_uint iID)
{
    auto iter = m_pInteractionDatas.find(iID);
    if (iter == m_pInteractionDatas.end())
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

const CAMERA_ANIMATION_DATA* CDataManager::Find_CameraAnimationData(_uint iCameraAnimationData)
{
    auto iter = m_CameraAnimationDatas.find(iCameraAnimationData);
    if (iter == m_CameraAnimationDatas.end())
        return nullptr;

    return &iter->second;
}

const CINEMATIC_DESC* CDataManager::Find_CinematicData(_uint iCinematicDataID)
{
    auto iter = m_CinematicDatas.find(iCinematicDataID);
    if (iter == m_CinematicDatas.end())
        return nullptr;

    return &iter->second;
}

void CDataManager::Save_CameraAnimationData()
{
    if (m_CameraAnimationDatas.size() == 0)
        return;

    for (auto& pData : m_CameraAnimationDatas)
    {
        Json jArray = Json::array();
        Json jData;

        jData["iCameraAnimationID"] = pData.first;
        jData["iCameraAnimationFlag"] = pData.second.iCameraAnimationFlag;
        jData["fBaseFOV"] = pData.second.fBaseFOV;
        jData["vBaseCameraPivot"] = { pData.second.vBaseCameraPivot.x, pData.second.vBaseCameraPivot.y, pData.second.vBaseCameraPivot.z};
        jData["vBaseBonePosition"] = { pData.second.vBaseBonePosition.x, pData.second.vBaseBonePosition.y, pData.second.vBaseBonePosition.z};
        jData["vBaseBoneRotation"] = { pData.second.vBaseBoneRotation.x, pData.second.vBaseBoneRotation.y, pData.second.vBaseBoneRotation.z};
        jData["szCameraAnimationName"] = pData.second.szCameraAnimationName;

        for (auto& pMember : pData.second.FOVTrackList)
        {
            Json jMember;

            jMember["fTrackPosition"] = pMember.fTrackPosition;
            jMember["vTrackValue"] = { pMember.vTrackValue.x, pMember.vTrackValue.y, pMember.vTrackValue.z };
            jMember["fTangentStart"] = pMember.fTangentStart;
            jMember["fTangentEnd"] = pMember.fTangentEnd;
            jMember["iInterpolationFlag"] = pMember.iInterpolationFlag;
            
            jData["FOVTrackList"].push_back(jMember);
        }

        for (auto& pMember : pData.second.PivotTrackList)
        {
            Json jMember;

            jMember["fTrackPosition"] = pMember.fTrackPosition;
            jMember["vTrackValue"] = { pMember.vTrackValue.x, pMember.vTrackValue.y, pMember.vTrackValue.z };
            jMember["fTangentStart"] = pMember.fTangentStart;
            jMember["fTangentEnd"] = pMember.fTangentEnd;
            jMember["iInterpolationFlag"] = pMember.iInterpolationFlag;
            
            jData["PivotTrackList"].push_back(jMember);
        }

        for (auto& pMember : pData.second.BonePositionTrackList)
        {
            Json jMember;

            jMember["fTrackPosition"] = pMember.fTrackPosition;
            jMember["vTrackValue"] = { pMember.vTrackValue.x, pMember.vTrackValue.y, pMember.vTrackValue.z };
            jMember["fTangentStart"] = pMember.fTangentStart;
            jMember["fTangentEnd"] = pMember.fTangentEnd;
            jMember["iInterpolationFlag"] = pMember.iInterpolationFlag;
            
            jData["BonePositionTrackList"].push_back(jMember);
        }

        for (auto& pMember : pData.second.BoneRotationTrackList)
        {
            Json jMember;

            jMember["fTrackPosition"] = pMember.fTrackPosition;
            jMember["vTrackValue"] = { pMember.vTrackValue.x, pMember.vTrackValue.y, pMember.vTrackValue.z };
            jMember["fTangentStart"] = pMember.fTangentStart;
            jMember["fTangentEnd"] = pMember.fTangentEnd;
            jMember["iInterpolationFlag"] = pMember.iInterpolationFlag;
            
            jData["BoneRotationTrackList"].push_back(jMember);
        }

        jArray.push_back(jData);

        _wstring szFilePath = TEXT("../../Client/Bin/DataFiles/CameraData/");

        szFilePath += to_wstring(pData.first);
        szFilePath += TEXT(".json");

        _char szPath[MAX_PATH]{};
        CStringHelper::ConvertWideToUTF(szFilePath.c_str(), szPath);

        CJsonParser::SaveJsonData(szPath, jArray);
    }
}

void CDataManager::Save_CinematicData()
{
    if (m_CinematicDatas.size() == 0)
        return;

    for (auto& pData : m_CinematicDatas)
    {
        Json jArray = Json::array();
        Json jData;

        jData["iCinematicID"] = pData.first;
        jData["szCinematicName"] = pData.second.szCinematicName;

        for (auto& pMember : pData.second.CinematicNodeTrackList)
        {
            Json jMember;

            jMember["eState"] = pMember.eState;
            jMember["fTrackPosition"] = pMember.fTrackPosition;
            jMember["szObjectTag"] = pMember.szObjectTag;
            jMember["iActiveIndex"] = pMember.iActiveIndex;

            jData["CinematicNodeTrackList"].push_back(jMember);
        }

        jArray.push_back(jData);

        _wstring szFilePath = TEXT("../../Client/Bin/DataFiles/CinematicData/");

        szFilePath += to_wstring(pData.first);
        szFilePath += TEXT(".json");

        _char szPath[MAX_PATH]{};
        CStringHelper::ConvertWideToUTF(szFilePath.c_str(), szPath);

        CJsonParser::SaveJsonData(szPath, jArray);
    }
}

#ifdef _DEBUG

map<_uint, CAMERA_ANIMATION_DATA>* CDataManager::Get_CameraAnimationMap()
{
    return &m_CameraAnimationDatas;
}

map<_uint, CINEMATIC_DESC>* CDataManager::Get_CinematicDataMap()
{
    return &m_CinematicDatas;
}

#endif

HRESULT CDataManager::LoadNaytibaData(void* pArg)
{
    THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
    vector<string> BossDataList; 
    BossDataList.reserve(1000);

    CStringHelper::CSVRead("../Bin/DataFiles/NaytibaData/NaytibaData.csv", BossDataList);

    size_t iMaxSize = BossDataList.size();
    for (auto i = 19; i < iMaxSize;)
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

        BossDesc.iMaxHealth = static_cast<_float>(atoi(BossDataList[i++].c_str()));
        BossDesc.iMaxShield = static_cast<_float>(atoi(BossDataList[i++].c_str()));
        BossDesc.iMaxStamina = static_cast<_float>(atoi(BossDataList[i++].c_str()));
        BossDesc.fMoveSpeed = static_cast<_float>(atoi(BossDataList[i++].c_str()));

        BossDesc.fAttackCoolTime = static_cast<_float>(atoi(BossDataList[i++].c_str()));
        BossDesc.fAttackRange = static_cast<_float>(atoi(BossDataList[i++].c_str()));
        BossDesc.fLerpRatio = static_cast<_float>(atof(BossDataList[i++].c_str()));

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

HRESULT CDataManager::LoadInteractionData(void* pArg)
{
    THREAD_DESC* Desc = static_cast<THREAD_DESC*>(pArg);
    Json InterDatas{};

    CJsonParser::ReadJsonData("../../Client/Bin/DataFiles/InteractionData/InteractionData.json", InterDatas);

    for (auto& iter : InterDatas["Interactions"].items())
    {
        _uint id = stoi(iter.key());

        auto& jInfo = iter.value();

        INTERACTION_DATA desc{};
        desc.iID = id;

        // 문자열 변환
        {
            string name = jInfo["Name"];
            string text = jInfo["Text"];
            desc.szObjectTag = UTF8ToWString(jInfo["Name"]);
            desc.szInteractionText = UTF8ToWString(jInfo["Text"]);
        }

        desc.fInteractionTime = jInfo["CoolTime"];

        // Pivot
        desc.vUIPivot.x = jInfo["Pivot"]["x"];
        desc.vUIPivot.y = jInfo["Pivot"]["y"];
        desc.vUIPivot.z = jInfo["Pivot"]["z"];

        // Type
        desc.eType = (INTERACTION_TYPE)jInfo["Type"];

        m_pInteractionDatas[id] = desc;
    }

    return S_OK;
}

HRESULT CDataManager::LoadSkillData()
{
    vector<string> SkillDataList;
    SkillDataList.reserve(1000);

    CStringHelper::CSVRead("../../Client/Bin/DataFiles/SkillData/SkillData.csv", SkillDataList);
    size_t iMaxSize = SkillDataList.size();

    for (auto i = 11; i < iMaxSize;)
    {
        CHARACTER_SKILL_DESC SkillDesc = {};
        SkillDesc.iSkillID = atoi(SkillDataList[i++].c_str());
        strcpy_s(SkillDesc.szAnimationName, SkillDataList[i++].c_str());
        strcpy_s(SkillDesc.szHitAnimationName, SkillDataList[i++].c_str());
        strcpy_s(SkillDesc.szLinkBoneName, SkillDataList[i++].c_str());

        SkillDesc.iSkillDamage = atoi(SkillDataList[i++].c_str());
        SkillDesc.fRange = static_cast<_float>(atof(SkillDataList[i++].c_str()));
        SkillDesc.vHitBoxExtents.x = static_cast<_float>(atof(SkillDataList[i++].c_str()));
        SkillDesc.vHitBoxExtents.y = static_cast<_float>(atof(SkillDataList[i++].c_str()));
        SkillDesc.vHitBoxExtents.z = static_cast<_float>(atof(SkillDataList[i++].c_str()));
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

HRESULT CDataManager::LoadCameraAnimationData(void* pArg)
{
    //	- 카메라 애니메이션 관리하는 데이터파일
    //  < 카메라 Data 파일 >
    //      카메라 ID
    //      카메라 플래그(본부착, 전환시 보간여부 등)
    //      기본 FOV
    //      기본 카메라 LookAt 포인트
    //      카메라본 기본 위치(플래그 활성 시)
    //      카메라본 기본 각도(플래그 활성 시)
    //      카메라본 애니메이션 이름
    //      FOV 채널
    //      카메라 LookAt 포인트 채널
    //      카메라본 위치 채널
    //      카메라본 각도 채널


    // _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
    _finddatai64_t  fd;

    // _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
    intptr_t handle = _findfirst64("../Bin/DataFiles/CameraData/*.json*", &fd);

    if (handle == -1)
        return S_OK;

    int iResult = 0;

    _wstring szFrontPath = TEXT("../Bin/DataFiles/CameraData/");

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
            CAMERA_ANIMATION_DATA CameraAnimationData;

            CameraAnimationData.iCameraAnimationID = pAnim["iCameraAnimationID"].get<_int>();
            CameraAnimationData.iCameraAnimationFlag = pAnim["iCameraAnimationFlag"].get<_int>();
            CameraAnimationData.fBaseFOV = pAnim["fBaseFOV"].get<_float>();

            CameraAnimationData.vBaseCameraPivot = {
               pAnim["vBaseCameraPivot"][0].get<_float>(),
               pAnim["vBaseCameraPivot"][1].get<_float>(),
               pAnim["vBaseCameraPivot"][2].get<_float>()
            };

            CameraAnimationData.vBaseBonePosition = {
               pAnim["vBaseBonePosition"][0].get<_float>(),
               pAnim["vBaseBonePosition"][1].get<_float>(),
               pAnim["vBaseBonePosition"][2].get<_float>()
            };

            CameraAnimationData.vBaseBoneRotation = {
               pAnim["vBaseBoneRotation"][0].get<_float>(),
               pAnim["vBaseBoneRotation"][1].get<_float>(),
               pAnim["vBaseBoneRotation"][2].get<_float>()
            };

            strcpy_s(CameraAnimationData.szCameraAnimationName, pAnim["szCameraAnimationName"].get<string>().c_str());


            for (auto& pFOVTrack : pAnim["FOVTrackList"])
            {
                CAMERA_TRACK_DESC TrackDesc = {};

                TrackDesc.fTrackPosition = pFOVTrack["fTrackPosition"].get<_float>();
                TrackDesc.vTrackValue = {
                    pFOVTrack["vTrackValue"][0].get<_float>(),
                    pFOVTrack["vTrackValue"][1].get<_float>(),
                    pFOVTrack["vTrackValue"][2].get<_float>()
                };
                TrackDesc.fTangentStart = pFOVTrack["fTangentStart"].get<_float>();
                TrackDesc.fTangentEnd = pFOVTrack["fTangentEnd"].get<_float>();
                TrackDesc.iInterpolationFlag = pFOVTrack["iInterpolationFlag"].get<_uint>();

                CameraAnimationData.FOVTrackList.push_back(TrackDesc);
            }

            for (auto& pPivotTrack : pAnim["PivotTrackList"])
            {
                CAMERA_TRACK_DESC TrackDesc = {};

                TrackDesc.fTrackPosition = pPivotTrack["fTrackPosition"].get<_float>();
                TrackDesc.vTrackValue = {
                    pPivotTrack["vTrackValue"][0].get<_float>(),
                    pPivotTrack["vTrackValue"][1].get<_float>(),
                    pPivotTrack["vTrackValue"][2].get<_float>()
                };
                TrackDesc.fTangentStart = pPivotTrack["fTangentStart"].get<_float>();
                TrackDesc.fTangentEnd = pPivotTrack["fTangentEnd"].get<_float>();
                TrackDesc.iInterpolationFlag = pPivotTrack["iInterpolationFlag"].get<_uint>();

                CameraAnimationData.PivotTrackList.push_back(TrackDesc);
            }

            for (auto& pBonePositionTrack : pAnim["BonePositionTrackList"])
            {
                CAMERA_TRACK_DESC TrackDesc = {};

                TrackDesc.fTrackPosition = pBonePositionTrack["fTrackPosition"].get<_float>();
                TrackDesc.vTrackValue = {
                    pBonePositionTrack["vTrackValue"][0].get<_float>(),
                    pBonePositionTrack["vTrackValue"][1].get<_float>(),
                    pBonePositionTrack["vTrackValue"][2].get<_float>()
                };
                TrackDesc.fTangentStart = pBonePositionTrack["fTangentStart"].get<_float>();
                TrackDesc.fTangentEnd = pBonePositionTrack["fTangentEnd"].get<_float>();
                TrackDesc.iInterpolationFlag = pBonePositionTrack["iInterpolationFlag"].get<_uint>();

                CameraAnimationData.BonePositionTrackList.push_back(TrackDesc);
            }

            for (auto& pBoneRotationTrack : pAnim["BoneRotationTrackList"])
            {
                CAMERA_TRACK_DESC TrackDesc = {};

                TrackDesc.fTrackPosition = pBoneRotationTrack["fTrackPosition"].get<_float>();
                TrackDesc.vTrackValue = {
                    pBoneRotationTrack["vTrackValue"][0].get<_float>(),
                    pBoneRotationTrack["vTrackValue"][1].get<_float>(),
                    pBoneRotationTrack["vTrackValue"][2].get<_float>()
                };
                TrackDesc.fTangentStart = pBoneRotationTrack["fTangentStart"].get<_float>();
                TrackDesc.fTangentEnd = pBoneRotationTrack["fTangentEnd"].get<_float>();
                TrackDesc.iInterpolationFlag = pBoneRotationTrack["iInterpolationFlag"].get<_uint>();

                CameraAnimationData.BoneRotationTrackList.push_back(TrackDesc);
            }


            m_CameraAnimationDatas.emplace(CameraAnimationData.iCameraAnimationID, CameraAnimationData);
        }


        //_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
        iResult = _findnext64(handle, &fd);
        Safe_Delete_Array(pFileName);
    }


    return S_OK;
}

HRESULT CDataManager::LoadCinematicData(void* pArg)
{
    // _finddata_t : <io.h>���� �����ϸ� ���� ������ �����ϴ� ����ü
    _finddatai64_t  fd;

    // _findfirst : <io.h>���� �����ϸ� ����ڰ� ������ ��� ������ ���� ù ��° ������ ã�� �Լ�
    intptr_t handle = _findfirst64("../Bin/DataFiles/CinematicData/*.json*", &fd);

    if (handle == -1)
        return S_OK;

    int iResult = 0;

    _wstring szFrontPath = TEXT("../Bin/DataFiles/CinematicData/");

    while (iResult != -1)
    {
        int iLength = strlen(fd.name) + 1;
        WCHAR* pFileName = new WCHAR[iLength];
        ZeroMemory(pFileName, sizeof(WCHAR) * iLength);

        // �ƽ�Ű �ڵ� ���ڿ��� �����ڵ� ���ڿ��� ��ȯ�����ִ� �Լ�
        MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pFileName, iLength);

        _wstring szFullPath = szFrontPath + pFileName;
        _wstring szFilePath = pFileName;
        Json jCinematic;

        _char szPath[MAX_PATH]{};
        CStringHelper::ConvertWideToUTF(szFullPath.c_str(), szPath);

        CJsonParser::ReadJsonData(szPath, jCinematic);

        for (auto& pCinematic : jCinematic)
        {
            CINEMATIC_DESC CinematicData;

            CinematicData.iCinematicID = pCinematic["iCinematicID"].get<_int>();

            strcpy_s(CinematicData.szCinematicName, pCinematic["szCinematicName"].get<string>().c_str());


            for (auto& pCinematicNodeTrack : pCinematic["CinematicNodeTrackList"])
            {
                CINEMATIC_NODE_DESC CinematicNodeDesc = {};

                CinematicNodeDesc.eState = static_cast<CINEMATICNODE_STATE>(pCinematicNodeTrack["eState"].get<_int>());
                CinematicNodeDesc.fTrackPosition = pCinematicNodeTrack["fTrackPosition"].get<_float>();

                strcpy_s(CinematicNodeDesc.szObjectTag, pCinematicNodeTrack["szObjectTag"].get<string>().c_str());

                CinematicNodeDesc.iActiveIndex = pCinematicNodeTrack["iActiveIndex"].get<_uint>();


                CinematicData.CinematicNodeTrackList.push_back(CinematicNodeDesc);
            }

            m_CinematicDatas.emplace(CinematicData.iCinematicID, CinematicData);
        }


        //_findnext : <io.h>���� �����ϸ� ���� ��ġ�� ������ ã�� �Լ�, ���̻� ���ٸ� -1�� ����
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
        BetaSkillDesc.iRequiredBetaGauge = 8;
        break;
    //테스트 용으로 0 처리
    case 1005:
        BetaSkillDesc.iRequiredBetaGauge = 8;
        break;
    default:
        BetaSkillDesc.iRequiredBetaGauge = 12;
        break;
    }

    m_pBetaSkills.emplace(iSkillID, BetaSkillDesc);

    return S_OK;
}

_wstring CDataManager::UTF8ToWString(const string& str)
{
    if (str.empty()) return {};

    _uint size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    _wstring result(size_needed - 1, 0); // null 제외
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size_needed);
    return result;
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
