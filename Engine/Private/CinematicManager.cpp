#include "CinematicManager.h"

#include "CutScene.h"
#include "StringHelper.h"

CCinematicManager::CCinematicManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    m_pDevice(pDevice),
    m_pContext(pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CCinematicManager::ADD_CutSceneData(const WCHAR* szSceneTag, const WCHAR* szTag, CCinemaTrack* pData)
{
    CCutScene* pScene = nullptr;
    auto iter = m_SceneDatas.find(szSceneTag);

    if (iter == m_SceneDatas.end())
    {
        pScene = CCutScene::Create(m_pDevice, m_pContext);
        m_SceneDatas.emplace(szSceneTag, pScene);
    }
    else
        pScene = iter->second;

    return pScene->ADD_CutSceneData(szTag, pData);
}

const CCinemaTrack* CCinematicManager::GetCutSceneData(const WCHAR* szSceneTag, const WCHAR* szTag)
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
    ios_base::open_mode bFlag = ios::out | ios::trunc;

    ofstream ofs(szFilePath, bFlag);
    if (ofs.is_open())
    {
        _uint iNumScence = m_SceneDatas.size();      

        //여기서 씬에서 몇개의 오브젝트를 구성중인가를 받아온다.
        ofs << iNumScence << endl;
        for (auto& iter : m_SceneDatas)
        {
            // 가장뒤에서부터 문자를 찾아주는 함수
            auto p = wcsrchr(szFilePath, L'.');
            // 문자 위치 다시 지정
            wstring FullPath = wstring(szFilePath, p - szFilePath);
            FullPath += TEXT("/");
            FullPath += iter.first.c_str();
            FullPath += TEXT("_Track.bin");
            char LowStr[MAX_PATH] = {};

            CStringHelper::ConvertWideToUTF(FullPath.c_str(), LowStr);
            ofs << LowStr << endl;
        }
    }
    else
        E_FAIL;

    ofs.close();

    for (auto& iter : m_SceneDatas)
    {
        // 가장뒤에서부터 문자를 찾아주는 함수
        auto p = wcsrchr(szFilePath, L'.');
        // 문자 위치 다시 지정
        wstring FullPath = wstring(szFilePath, p - szFilePath);
        FullPath += TEXT("/");

        CreateDirectoryW(FullPath.c_str(), NULL);
        FullPath += iter.first.c_str();
        FullPath += TEXT("_Track.bin");

        if (FAILED(iter.second->Export(FullPath.c_str())))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CCinematicManager::LoadCutSceneData(const WCHAR* szFilePath)
{
    ios_base::open_mode bFlag = ios::in;

    ifstream ifs(szFilePath, bFlag);
    if (ifs.is_open())
    {
        _uint iNumScence = {};
        ifs >> iNumScence;

        char szUFilePath[MAX_PATH] = {};

        WCHAR szWideFilePath[MAX_PATH] = {};
        WCHAR szFileName[MAX_PATH] = {};
        WCHAR szExtents[MAX_PATH] = {};

        for (_uint i = 0; i < iNumScence; ++i)
        {
            ifs >> szUFilePath;
            CStringHelper::ConvertUTFToWide(szUFilePath, szWideFilePath);
            _wsplitpath_s(szWideFilePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExtents, MAX_PATH);

            auto iter = m_SceneDatas.find(szExtents);
            if (iter == m_SceneDatas.end())
            {
                auto pCinemaData = CCutScene::Create(m_pDevice, m_pContext, szWideFilePath);
                m_SceneDatas.emplace(szFileName, pCinemaData);
            }
            else
            {
                if (FAILED(iter->second->Import(szWideFilePath)))
                    return E_FAIL;
            }
        }
    }
    ifs.close();

    return S_OK;
}

const unordered_map<_wstring, CCinemaTrack*>* CCinematicManager::GetSceneAllDatas(const WCHAR* szSceneTag)
{
    auto iter = m_SceneDatas.find(szSceneTag);
    if (iter == m_SceneDatas.end())
        return nullptr;

    return iter->second->GetAllSceneData();
}

CCinematicManager* CCinematicManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CCinematicManager(pDevice, pContext);
}

void CCinematicManager::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    for (auto& iter : m_SceneDatas)
        Safe_Release(iter.second);

    m_SceneDatas.clear();
}
