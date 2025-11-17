#include "pch.h"
#include "Data_Manager.h"

#include "GameInstance.h"

CData_Manager::CData_Manager()
{
}

HRESULT CData_Manager::Initalize()
{
    if (FAILED(LoadAnimNotifyData()))
        return E_FAIL;


    return S_OK;
}

const ANIM_NOTIFY* CData_Manager::Find_AnimationNotifyData(_wstring szAnimationTag)
{
   // auto iter = m_pSkillDatas.find(iSkillID);
   // if (iter == m_pSkillDatas.end())
   //     return nullptr;

    // return &iter->second;
    return nullptr;
}

HRESULT CData_Manager::LoadAnimNotifyData(void* pArg)
{
    //vector<string> SkillDataList;
    //SkillDataList.reserve(1000);
    //
    //CStringHelper::CSVRead("../Bin/DataFiles/SkillData/SkillData.csv", SkillDataList);
    //
    //CHARACTER_SKILL_DESC SkillDesc = {};
    //size_t iMaxSize = SkillDataList.size();
    //
    //for (auto i = 5; i < iMaxSize; i += 5)
    //{
    //    SkillDesc.iSkillID = atoi(SkillDataList[i].c_str());
    //    strcpy_s(SkillDesc.szAnimationName, SkillDataList[i + 1].c_str());
    //
    //    SkillDesc.iSkillDamage = atoi(SkillDataList[i + 2].c_str());
    //    SkillDesc.eDirection = DIRECTION(atoi(SkillDataList[i + 3].c_str()));
    //    SkillDesc.eSkillType = SKILL_TYPE(atoi(SkillDataList[i + 4].c_str()));
    //
    //    m_pSkillDatas.emplace(SkillDesc.iSkillID, SkillDesc);
    //}

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
