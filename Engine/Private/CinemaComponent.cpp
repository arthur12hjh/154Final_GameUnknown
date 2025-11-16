#include "CinemaComponent.h"

CCinemaComponent::CCinemaComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CComponent(pDevice, pContext)
{

}

CCinemaComponent::CCinemaComponent(const CCinemaComponent& Prototype) :
    CComponent(Prototype),
    m_pKeyFrameList(Prototype.m_pKeyFrameList)
{
    
}

HRESULT CCinemaComponent::Initialize_Prototype(const char* CinemaFilePath)
{
    if (0 != strcmp("", CinemaFilePath))
    {

    }
    else
    {
        if (FAILED(Read_FileData(CinemaFilePath)))
            return E_FAIL;
    }

    m_pKeyFrameList = new vector<KEYFRAME>();
    if (nullptr == m_pKeyFrameList)
        return E_FAIL;
    
    m_pKeyFrameList->reserve(300);
    return S_OK;
}

HRESULT CCinemaComponent::Initialize(void* pArg)
{
    return S_OK;
}

void CCinemaComponent::Insert_KeyFrame(KEYFRAME KeyFrame, _int iIndex)
{
    if(0 > iIndex)
        m_pKeyFrameList->push_back(KeyFrame);
    else
        m_pKeyFrameList->insert(m_pKeyFrameList->begin() + iIndex, KeyFrame);
}

void CCinemaComponent::Remove_KeyFrame(_uint iKeyFrameIndex)
{
    auto iter = m_pKeyFrameList->begin() + iKeyFrameIndex;

    if (iter == m_pKeyFrameList->end())
        return;

    m_pKeyFrameList->erase(iter);
}

size_t CCinemaComponent::GetNumKeyFrame()
{
    return m_pKeyFrameList->size();
}

HRESULT CCinemaComponent::Save_FileData(const char* CinemaFilePath)
{
    ios_base::openmode flag;
    flag = ios::out | ios::trunc;

    ofstream ofs(CinemaFilePath, flag);
    if (ofs.is_open())
    {
        _uint iNumKeyFrame = {};
        ofs << iNumKeyFrame;

        KEYFRAME KeyFrame = {};
        for (_uint i = 0; i < iNumKeyFrame; ++i)
            ofs << reinterpret_cast<char*>(&KeyFrame);
    }

    ofs.close();
    return S_OK;
}

HRESULT CCinemaComponent::Read_FileData(const char* CinemaFilePath)
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
            ifs >> reinterpret_cast<char*>(&KeyFrame);
    }
    
    ifs.close();
    return S_OK;
}

CCinemaComponent* CCinemaComponent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* CinemaFilePath)
{
    CCinemaComponent* pCinemaCom = new CCinemaComponent(pDevice, pContext);
    if (FAILED(pCinemaCom->Initialize_Prototype(CinemaFilePath)))
    {
        Safe_Release(pCinemaCom);
        MSG_BOX("Create Fail : Cinema Component");
    }
    return pCinemaCom;
}

CComponent* CCinemaComponent::Clone(void* pArg)
{
    CCinemaComponent* pCinemaCom = new CCinemaComponent(*this);
    if (FAILED(pCinemaCom->Initialize(pArg)))
    {
        Safe_Release(pCinemaCom);
        MSG_BOX("Clone Fail : Cinema Component");
    }
    return pCinemaCom;
}

void CCinemaComponent::Free()
{
    __super::Free();

    if(false == m_isCloned)
        Safe_Delete(m_pKeyFrameList);
}
