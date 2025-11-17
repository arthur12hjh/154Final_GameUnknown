#include "CinemaData.h"

CCinemaData::CCinemaData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CComponent(pDevice, pContext)
{

}

CCinemaData::CCinemaData(const CCinemaData& Prototype) :
    CComponent(Prototype),
    m_pKeyFrameList(Prototype.m_pKeyFrameList)
{
    
}

HRESULT CCinemaData::Initialize_Prototype(const WCHAR* CinemaFilePath)
{
    if (!lstrcmp(L"", CinemaFilePath))
    {

    }
    else
    {
        if (FAILED(Read_FileData(CinemaFilePath)))
            return E_FAIL;
    }

    m_pKeyFrameList.reserve(300);
    return S_OK;
}

HRESULT CCinemaData::Initialize(void* pArg)
{
    return S_OK;
}

void CCinemaData::Insert_KeyFrame(KEYFRAME* KeyFrame, _int iIndex)
{
    if(0 > iIndex)
        m_pKeyFrameList.push_back(KeyFrame);
    else
        m_pKeyFrameList.insert(m_pKeyFrameList.begin() + iIndex, KeyFrame);
}

void CCinemaData::Remove_KeyFrame(_uint iKeyFrameIndex)
{
    auto iter = m_pKeyFrameList.begin() + iKeyFrameIndex;

    if (iter == m_pKeyFrameList.end())
        return;

    m_pKeyFrameList.erase(iter);
}

size_t CCinemaData::GetNumKeyFrame()
{
    return m_pKeyFrameList.size();
}

HRESULT CCinemaData::Save_FileData(const WCHAR* CinemaFilePath)
{
    ios_base::openmode flag;
    flag = ios::out | ios::binary | ios::trunc;

    ofstream ofs(CinemaFilePath, flag);
    if (ofs.is_open())
    {
        _uint iNumKeyFrame = {};
        ofs << iNumKeyFrame;

        for (_uint i = 0; i < iNumKeyFrame; ++i)
        {
            KEYFRAME KeyFrame = *m_pKeyFrameList[i];
            ofs << reinterpret_cast<char*>(&KeyFrame);
        }
    }

    ofs.close();
    return S_OK;
}

HRESULT CCinemaData::Read_FileData(const WCHAR* CinemaFilePath)
{
    ios_base::openmode flag;
    flag = ios::in | ios::binary;

    ifstream ifs(CinemaFilePath, flag);
    if (ifs.is_open())
    {
        _uint iNumKeyFrame = {};
        ifs >> iNumKeyFrame;

        KEYFRAME KeyFrame = {};
        for (_uint i = 0; i < iNumKeyFrame; ++i)
        {
            ifs >> reinterpret_cast<char*>(&KeyFrame);
            m_pKeyFrameList.push_back(new KEYFRAME(KeyFrame));
        }
    }
    
    ifs.close();
    return S_OK;
}

CCinemaData* CCinemaData::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const WCHAR* CinemaFilePath)
{
    CCinemaData* pCinemaCom = new CCinemaData(pDevice, pContext);
    if (FAILED(pCinemaCom->Initialize_Prototype(CinemaFilePath)))
    {
        Safe_Release(pCinemaCom);
        MSG_BOX("Create Fail : Cinema Component");
    }
    return pCinemaCom;
}

CComponent* CCinemaData::Clone(void* pArg)
{
    CCinemaData* pCinemaCom = new CCinemaData(*this);
    if (FAILED(pCinemaCom->Initialize(pArg)))
    {
        Safe_Release(pCinemaCom);
        MSG_BOX("Clone Fail : Cinema Component");
    }
    return pCinemaCom;
}

void CCinemaData::Free()
{
    __super::Free();

    if (false == m_isCloned)
    {
        for (auto& iter : m_pKeyFrameList)
            Safe_Delete(iter);
    }
       
}
