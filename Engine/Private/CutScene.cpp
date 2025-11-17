#include "CutScene.h"

#include "CinemaData.h"
#include "StringHelper.h"

CCutScene::CCutScene()
{
}

HRESULT CCutScene::ADD_CutSceneData(const WCHAR* szTag, CCinemaData* pCinemaComponent)
{
    if (nullptr == pCinemaComponent)
        return E_FAIL;

    auto iter = m_SceneDatas.find(szTag);
    if (iter == m_SceneDatas.end())
    {
        m_SceneDatas.emplace(szTag, pCinemaComponent);
    }
    else
        return E_FAIL;

    return S_OK;
}

const CCinemaData* CCutScene::GetCutSceneData(const WCHAR* szTag)
{
    auto iter = m_SceneDatas.find(szTag);
    if (iter == m_SceneDatas.end())
        return nullptr;

    return iter->second;
}

const unordered_map<_wstring, CCinemaData*>* CCutScene::GetAllSceneData()
{
    return &m_SceneDatas;
}

HRESULT CCutScene::Export(const WCHAR* szFilePath)
{
    ios_base::open_mode bFlag = ios::out | ios::trunc;
    ofstream ifs(szFilePath, bFlag);

    if (ifs.is_open())
    {
        _uint iNumScence = m_SceneDatas.size();
        //여기서 씬에서 몇개의 오브젝트를 구성중인가를 받아온다.
        ifs << iNumScence << endl;
        for (auto& iter : m_SceneDatas)
        {
            // 여기서 데이터별로 데이터를 저장시킬거임
            // 파일 경로 + 이름
           
            // 가장뒤에서부터 문자를 찾아주는 함수
            auto p = wcsrchr(szFilePath, L'.');
            // 문자 위치 다시 지정
            wstring FullPath = wstring(szFilePath, p - szFilePath);
            FullPath += TEXT("/");
            FullPath += iter.first.c_str();
            FullPath += TEXT(".bin");
            char LowStr[MAX_PATH] = {};

            CStringHelper::ConvertWideToUTF(FullPath.c_str(), LowStr);
            ifs << LowStr << endl;
        }
    }
    else
        E_FAIL;

    ifs.close();

    for (auto& iter : m_SceneDatas)
    {
        auto p = wcsrchr(szFilePath, L'.');
        // 문자 위치 다시 지정
        wstring FullPath = wstring(szFilePath, p - szFilePath);
        FullPath += TEXT("/");
        FullPath += iter.first.c_str();
        FullPath += TEXT(".bin");

        iter.second->Save_FileData(FullPath.c_str());
    }
       
    return S_OK;
}

HRESULT CCutScene::Import(const WCHAR* szFilePath)
{
    return S_OK;
}

CCutScene* CCutScene::Create()
{
    return new CCutScene();
}

void CCutScene::Free()
{
    __super::Free();


    for (auto& iter : m_SceneDatas)
        Safe_Release(iter.second);

    m_SceneDatas.clear();
}


