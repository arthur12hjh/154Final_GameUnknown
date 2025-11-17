#include "CutScene.h"

#include "CinemaTrack.h"
#include "StringHelper.h"

CCutScene::CCutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    m_pDevice(pDevice),
    m_pContext(pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CCutScene::Initailize(const WCHAR* szFilePath)
{
    if (0 == lstrcmp(szFilePath, L""))
    {

    }
    else
    {
        Import(szFilePath);
    }
    return S_OK;
}

HRESULT CCutScene::ADD_CutSceneData(const WCHAR* szTag, CCinemaTrack* pCinemaComponent)
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

const CCinemaTrack* CCutScene::GetCutSceneData(const WCHAR* szTag)
{
    auto iter = m_SceneDatas.find(szTag);
    if (iter == m_SceneDatas.end())
        return nullptr;

    return iter->second;
}

const unordered_map<_wstring, CCinemaTrack*>* CCutScene::GetAllSceneData()
{
    return &m_SceneDatas;
}

HRESULT CCutScene::Export(const WCHAR* szFilePath)
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
            // 여기서 데이터별로 데이터를 저장시킬거임
            // 파일 경로 + 이름
           
            // 가장뒤에서부터 문자를 찾아주는 함수
            auto p = wcsrchr(szFilePath, L'.');
            // 문자 위치 다시 지정
            wstring FullPath = wstring(szFilePath, p - szFilePath);
            FullPath += TEXT("_KeyFrame.bin");
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
        auto p = wcsrchr(szFilePath, L'.');
        // 문자 위치 다시 지정
        wstring FullPath = wstring(szFilePath, p - szFilePath);
        FullPath += TEXT("_KeyFrame.bin");

        iter.second->Save_FileData(FullPath.c_str());
    }
       
    return S_OK;
}

HRESULT CCutScene::Import(const WCHAR* szFilePath)
{
    ios_base::open_mode bFlag = ios::in;

    ifstream ifs(szFilePath, bFlag);
    if (ifs.is_open())
    {
        _uint iNumScence = m_SceneDatas.size();
        //여기서 씬에서 몇개의 오브젝트를 구성중인가를 받아온다.
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
                auto pCinemaData = CCinemaTrack::Create(m_pDevice, m_pContext, szWideFilePath);
                m_SceneDatas.emplace(szFileName, pCinemaData);
            }
            else
            {
                iter->second->Read_FileData(szWideFilePath);
            }
        }
    }
    else
        E_FAIL;

    ifs.close();

    return S_OK;
}

CCutScene* CCutScene::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const WCHAR* szFilePath)
{
    auto pCutScene = new CCutScene(pDevice, pContext);
    if (FAILED(pCutScene->Initailize(szFilePath)))
    {
        Safe_Release(pCutScene);
        MSG_BOX("Create Fail : Cut Scene");
    }

    return pCutScene;
}

void CCutScene::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    for (auto& iter : m_SceneDatas)
        Safe_Release(iter.second);

    m_SceneDatas.clear();
}


