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
    Json jAnimNotifies;

    if (m_AnimationNotifyDatas.size() == 0)
		return S_OK;

    for (auto& pData : m_AnimationNotifyDatas)
    {
        Json jData;
        _char szText[MAX_PATH];

        CStringHelper::ConvertWideToUTF(pData.first.c_str(), szText);
        jData["szAnimationTag"] = szText;

        for (auto& pMember : pData.second)
        {
            ///  -> string szNotifyTag
            ///  -> string szNotifyArg01
            ///  -> string szNotifyArg02
            ///  -> string szSocketTag
            ///  -> _float3 vNotifyPosition
            ///  -> _float3 vNotifyRotation
            ///  -> _uint  iNotifyKeyFrame
            Json jMember;
            jMember["szNotifyTag"] = pMember.szNotifyTag.c_str();
            jMember["szNotifyArg01"] = pMember.szNotifyArg01.c_str();
            jMember["szNotifyArg02"] = pMember.szNotifyArg02.c_str();
            jMember["szNotifyArg03"] = pMember.szNotifyArg03.c_str();
            jMember["szNotifyArg04"] = pMember.szNotifyArg04.c_str();
            jMember["szNotifyArg05"] = pMember.szNotifyArg05.c_str();
            jMember["szNotifyArg06"] = pMember.szNotifyArg06.c_str();
            jMember["szNotifyArg07"] = pMember.szNotifyArg07.c_str();
            jMember["szNotifyArg08"] = pMember.szNotifyArg08.c_str();
            
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

            jMember["szSocketTag"] = pMember.szSocketTag.c_str();

            jMember["vNotifyScale"] = {
               pMember.vNotifyScale.x,
               pMember.vNotifyScale.y,
               pMember.vNotifyScale.z
            };

            jMember["vNotifyPosition"] = {
                pMember.vNotifyPosition.x,
                pMember.vNotifyPosition.y,
                pMember.vNotifyPosition.z
			};
            jMember["vNotifyRotation"] = {
                pMember.vNotifyRotation.x,
                pMember.vNotifyRotation.y,
                pMember.vNotifyRotation.z
			};

            jMember["iNotifyKeyFrame"] = pMember.iNotifyKeyFrame;

            jData["AnimationNotifies"].push_back(jMember);
        }

        jAnimNotifies.push_back(jData);
    }

	_wstring szFilePath{};
    szFilePath = TEXT("../Bin/DataFiles/Animation/AnimationNotifyData.json");

	_char szPath[MAX_PATH]{};
    CStringHelper::ConvertWideToUTF(szFilePath.c_str(), szPath);

    CJsonParser::SaveJsonData(szPath, jAnimNotifies);

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
        
    Json jAnim;

    wstring szFilePath{};
	szFilePath = TEXT("../Bin/DataFiles/Animation/AnimationNotifyData.json");

	_char szPath[MAX_PATH]{};
	CStringHelper::ConvertWideToUTF(szFilePath.c_str(), szPath);

	CJsonParser::ReadJsonData(szPath, jAnim);

    for(auto& pAnim : jAnim)
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
