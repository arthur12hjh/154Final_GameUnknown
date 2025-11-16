#include "CinematicManager.h"

#include "CutScene.h"

CCinematicManager::CCinematicManager()
{
}

HRESULT CCinematicManager::ADD_CutSceneData(const WCHAR* szSceneTag, const WCHAR* szTag, CCinemaData* pData)
{
    CCutScene* pScene = nullptr;
    auto iter = m_SceneDatas.find(szSceneTag);

    if (iter == m_SceneDatas.end())
    {
        pScene = CCutScene::Create();
        m_SceneDatas.emplace(szSceneTag, pScene);
    }
    else
        pScene = iter->second;

    return pScene->ADD_CutSceneData(szTag, pData);
}

const CCinemaData* CCinematicManager::GetCutSceneData(const WCHAR* szSceneTag, const WCHAR* szTag)
{
    auto iter = m_SceneDatas.find(szSceneTag);
    if (iter == m_SceneDatas.end())
        return nullptr;

    return iter->second->GetCutSceneData(szTag);
}

const unordered_map<_wstring, CCutScene*>* CCinematicManager::GetAllScenes()
{
    return &m_SceneDatas;
}

HRESULT CCinematicManager::SaveCutSceneData(const WCHAR* szFilePath)
{
    for (auto& iter : m_SceneDatas)
    {
        // 가장뒤에서부터 문자를 찾아주는 함수
        auto p = wcsrchr(szFilePath, L'.');
        // 문자 위치 다시 지정
        wstring FullPath = wstring(szFilePath, p - szFilePath);
        FullPath += TEXT("/");
        FullPath += iter.first.c_str();
        FullPath += TEXT(".bin");

        if (FAILED(iter.second->Export(FullPath.c_str())))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CCinematicManager::LoadCutSceneData(const WCHAR* szFilePath)
{
    for (auto& iter : m_SceneDatas)
    {
        if (FAILED(iter.second->Import(szFilePath)))
            return E_FAIL;
    }
    return S_OK;
}

const unordered_map<_wstring, CCinemaData*>* CCinematicManager::GetSceneAllDatas(const WCHAR* szSceneTag)
{
    auto iter = m_SceneDatas.find(szSceneTag);
    if (iter == m_SceneDatas.end())
        return nullptr;

    return iter->second->GetAllSceneData();
}

CCinematicManager* CCinematicManager::Create()
{
    return new CCinematicManager();
}

void CCinematicManager::Free()
{
    __super::Free();

    for (auto& iter : m_SceneDatas)
        Safe_Release(iter.second);

    m_SceneDatas.clear();
}
