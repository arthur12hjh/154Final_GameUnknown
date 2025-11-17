#include "CinemaTrack.h"

#include "Transform.h"

CCinemaTrack::CCinemaTrack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CComponent(pDevice, pContext)
{

}

CCinemaTrack::CCinemaTrack(const CCinemaTrack& Prototype) :
    CComponent(Prototype),
    m_pKeyFrameList(Prototype.m_pKeyFrameList)
{
    
}

HRESULT CCinemaTrack::Initialize_Prototype(const WCHAR* CinemaFilePath)
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

HRESULT CCinemaTrack::Initialize(void* pArg)
{
    return S_OK;
}

void CCinemaTrack::SetPlayTime(_float fTime)
{
    m_fTime.y = fTime;
}

void CCinemaTrack::Insert_KeyFrame(KEYFRAME* KeyFrame, _int iIndex)
{
    if(0 > iIndex)
        m_pKeyFrameList.push_back(KeyFrame);
    else
        m_pKeyFrameList.insert(m_pKeyFrameList.begin() + iIndex, KeyFrame);
}

void CCinemaTrack::Remove_KeyFrame(_uint iKeyFrameIndex)
{
    auto iter = m_pKeyFrameList.begin() + iKeyFrameIndex;

    if (iter == m_pKeyFrameList.end())
        return;

    Safe_Delete(*iter);
    m_pKeyFrameList.erase(iter);
}

size_t CCinemaTrack::GetNumKeyFrame()
{
    return m_pKeyFrameList.size();
}

_float CCinemaTrack::GetPlayTime()
{
    return m_fTime.y;
}

HRESULT CCinemaTrack::Save_FileData(const WCHAR* CinemaFilePath)
{
    ios_base::openmode flag;
    flag = ios::out | ios::binary | ios::trunc;

    char LowStr[MAX_PATH] = {};
    ofstream ofs(CinemaFilePath, flag);
    if (ofs.is_open())
    {
        ofs.write(reinterpret_cast<char*>(&m_fTime.y), sizeof(_uint));

        _uint iNumKeyFrame = m_pKeyFrameList.size();
        ofs.write(reinterpret_cast<char*>(&iNumKeyFrame), sizeof(_uint));

        for (_uint i = 0; i < iNumKeyFrame; ++i)
        {
            KEYFRAME KeyFrame = *m_pKeyFrameList[i];
            ofs.write(reinterpret_cast<char*>(&KeyFrame), sizeof(KEYFRAME));
        }
    }

    ofs.close();
    return S_OK;
}

HRESULT CCinemaTrack::Read_FileData(const WCHAR* CinemaFilePath)
{
    ios_base::openmode flag;
    flag = ios::in | ios::binary;

    ifstream ifs(CinemaFilePath, flag);
    if (ifs.is_open())
    {
        _uint iNumKeyFrame = {};
        ifs.read(reinterpret_cast<char*>(&m_fTime.y), sizeof(_uint));
        ifs.read(reinterpret_cast<char*>(&iNumKeyFrame), sizeof(_uint));

        KEYFRAME KeyFrame = {};
        for (_uint i = 0; i < iNumKeyFrame; ++i)
        {
            ifs.read(reinterpret_cast<char*>(&KeyFrame), sizeof(KEYFRAME));
            m_pKeyFrameList.push_back(new KEYFRAME(KeyFrame));
        }
    }
    
    ifs.close();
    return S_OK;
}

void CCinemaTrack::Set_Animation(_float PlayTime)
{
    m_iIndex = 0;
    m_fTime = { 0.f, PlayTime / _uint(m_pKeyFrameList.size()) };
    m_bIsAnimation = true;
}

_bool CCinemaTrack::Play_Animation(CTransform* pTransform, _float fTimeDelta)
{
    _uint iNumFrame = _uint(m_pKeyFrameList.size());
    m_fTime.x += fTimeDelta;
    if (m_fTime.x >= m_fTime.y)
    {
        m_fTime.x = 0.f;
        m_iIndex++;

        if (iNumFrame - 1 <= m_iIndex)
        {
            m_bIsAnimation = false;
            m_fTime.x = m_fTime.y;
            m_iIndex = iNumFrame - 2;
        }
    }

    _float fRatio = {};
    if (0.f == m_fTime.x)
        fRatio = 0.f;
    else
        fRatio = m_fTime.x / m_fTime.y;

    KEYFRAME* pCurFrame = (m_pKeyFrameList)[m_iIndex];
    KEYFRAME* pNextFrame = (m_pKeyFrameList)[m_iIndex + 1];

    auto pTranslateLerp = XMVectorLerp(XMLoadFloat3(&pCurFrame->vTranslation), XMLoadFloat3(&pNextFrame->vTranslation), fRatio);
    auto pRotationLerp = XMVectorLerp(XMLoadFloat4(&pCurFrame->vRotation), XMLoadFloat4(&pNextFrame->vRotation), fRatio);

    pTransform->Set_State(STATE::POSITION, pTranslateLerp);
    pTransform->Set_Rotation(pRotationLerp);
    return m_bIsAnimation ? false : true;
}

CCinemaTrack* CCinemaTrack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const WCHAR* CinemaFilePath)
{
    CCinemaTrack* pCinemaCom = new CCinemaTrack(pDevice, pContext);
    if (FAILED(pCinemaCom->Initialize_Prototype(CinemaFilePath)))
    {
        Safe_Release(pCinemaCom);
        MSG_BOX("Create Fail : Cinema Component");
    }
    return pCinemaCom;
}

CComponent* CCinemaTrack::Clone(void* pArg)
{
    CCinemaTrack* pCinemaCom = new CCinemaTrack(*this);
    if (FAILED(pCinemaCom->Initialize(pArg)))
    {
        Safe_Release(pCinemaCom);
        MSG_BOX("Clone Fail : Cinema Component");
    }
    return pCinemaCom;
}

void CCinemaTrack::Free()
{
    __super::Free();

    if (false == m_isCloned)
    {
        for (auto& iter : m_pKeyFrameList)
            Safe_Delete(iter);
    }
       
}
