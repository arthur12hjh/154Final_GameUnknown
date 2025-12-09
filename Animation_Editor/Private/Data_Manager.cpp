#include "pch.h"
#include "Data_Manager.h"

#include "GameInstance.h"

#include "JsonParser.h"
#include "StringHelper.h"

CData_Manager::CData_Manager()
{
}

HRESULT CData_Manager::Initalize()
{
    if (FAILED(LoadAnimNotifyData()))
        return E_FAIL;


    return S_OK;
}

HRESULT CData_Manager::Save_Data()
{
    if (m_AnimationNotifyDatas.size() == 0)
        return S_OK;

    for (auto& pData : m_AnimationNotifyDatas)
    {
        Json jArray = Json::array();   
        Json jData;                    

        _char szAnimTag[MAX_PATH];
        CStringHelper::ConvertWideToUTF(pData.first.c_str(), szAnimTag);
        jData["szAnimationTag"] = szAnimTag;

        jData["AnimationNotifies"] = Json::array();

        for (auto& pMember : pData.second)
        {
            Json jMember;

            jMember["szNotifyTag"] = pMember.szNotifyTag;
            jMember["szNotifyArg01"] = pMember.szNotifyArg01;
            jMember["szNotifyArg02"] = pMember.szNotifyArg02;
            jMember["szNotifyArg03"] = pMember.szNotifyArg03;
            jMember["szNotifyArg04"] = pMember.szNotifyArg04;
            jMember["szNotifyArg05"] = pMember.szNotifyArg05;
            jMember["szNotifyArg06"] = pMember.szNotifyArg06;
            jMember["szNotifyArg07"] = pMember.szNotifyArg07;
            jMember["szNotifyArg08"] = pMember.szNotifyArg08;

            jMember["iNumData01"] = pMember.iNumData01;
            jMember["iNumData02"] = pMember.iNumData02;
            jMember["iNumData03"] = pMember.iNumData03;
            jMember["iNumData04"] = pMember.iNumData04;
            jMember["iNumData05"] = pMember.iNumData05;
            jMember["iNumData06"] = pMember.iNumData06;
            jMember["iNumData07"] = pMember.iNumData07;
            jMember["iNumData08"] = pMember.iNumData08;

            jMember["fNumData01"] = pMember.fNumData01;
            jMember["fNumData02"] = pMember.fNumData02;
            jMember["fNumData03"] = pMember.fNumData03;
            jMember["fNumData04"] = pMember.fNumData04;

            jMember["szSocketTag"] = pMember.szSocketTag;

            jMember["vNotifyScale"] = { pMember.vNotifyScale.x, pMember.vNotifyScale.y, pMember.vNotifyScale.z };
            jMember["vNotifyPosition"] = { pMember.vNotifyPosition.x, pMember.vNotifyPosition.y, pMember.vNotifyPosition.z };
            jMember["vNotifyRotation"] = { pMember.vNotifyRotation.x, pMember.vNotifyRotation.y, pMember.vNotifyRotation.z };

            jMember["iNotifyKeyFrame"] = pMember.iNotifyKeyFrame;

            jData["AnimationNotifies"].push_back(jMember);
        }

        jArray.push_back(jData);

        _wstring szFilePath = TEXT("../../Client/Bin/DataFiles/Animation/");
        szFilePath += pData.first;
        szFilePath += TEXT(".json");

        _char szPath[MAX_PATH]{};
        CStringHelper::ConvertWideToUTF(szFilePath.c_str(), szPath);

        CJsonParser::SaveJsonData(szPath, jArray);
    }

    return S_OK;
}

const vector<ANIM_NOTIFY>* CData_Manager::Find_AnimationNotifyData(const _wstring& szAnimationTag)
{
   auto iter = m_AnimationNotifyDatas.find(szAnimationTag);
   if (iter == m_AnimationNotifyDatas.end())
       return nullptr;

    return &iter->second;
}

HRESULT CData_Manager::LoadAnimNotifyData(void* pArg)
{
    //	- 모든 애니메이션 관련 이벤트를 담당하는 ANIM_NOTIFY
    //  - 를 담고 있는 vector<ANIM_NOTIFY>
    //  - 들을 애니메이션 태그(_char*)로 구분짓는 map

    // _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
    _finddatai64_t  fd;

    // _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
    // json도 되려나 이거..
    intptr_t handle = _findfirst64("../../Client/Bin/DataFiles/Animation/*.json*", &fd);

    if (handle == -1)
        return S_OK;

    int iResult = 0;

    _wstring szFrontPath = TEXT("../../Client/Bin/DataFiles/Animation/");

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

CData_Manager* CData_Manager::Create()
{
    CData_Manager* pData_Manager = new CData_Manager();
    if (FAILED(pData_Manager->Initalize()))
    {
        Safe_Release(pData_Manager);
        MSG_BOX("Create Fail : Data Manager");
    }
    return pData_Manager;
}

void CData_Manager::Free()
{
    m_AnimationNotifyDatas.clear();
    __super::Free();
}
