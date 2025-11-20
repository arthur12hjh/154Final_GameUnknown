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
            jMember["szSocketTag"] = pMember.szSocketTag.c_str();
			jMember["bIsLocalPos"] = pMember.bIsLocalPos;
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
    __super::Free();
}
