#include "CameraManager.h"

#include "Camera.h"

HRESULT CCameraManager::Initialize()
{
    return S_OK;
}

void CCameraManager::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
    for (auto& iter : m_pCameras)
        iter.second->Priority_Update(fTimeDelta);
#else
    if (nullptr == m_pMainCamera)
        return;
    m_pMainCamera->Priority_Update(fTimeDelta);
#endif // _DEBUG

  
}

void CCameraManager::Update(_float fTimeDelta)
{
#ifdef _DEBUG
    for (auto& iter : m_pCameras)
        iter.second->Update(fTimeDelta);
#else
    if (nullptr == m_pMainCamera)
        return;
    m_pMainCamera->Update(fTimeDelta);
#endif // _DEBUG
}

void CCameraManager::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
    for (auto& iter : m_pCameras)
        iter.second->Late_Update(fTimeDelta);
#else
    if (nullptr == m_pMainCamera)
        return;
    m_pMainCamera->Late_Update(fTimeDelta);
#endif // _DEBUG
}

HRESULT CCameraManager::Add_Camera(const WCHAR* szCameraTag, CCamera* pCamera)
{
    CCamera* pFindCamera = Find_Camera(szCameraTag);
    if (nullptr == pFindCamera)
        m_pCameras.emplace(szCameraTag, pCamera);
    else
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraManager::Remove_Camera(const WCHAR* szCameraTag)
{
    auto iter = m_pCameras.find(szCameraTag);
    if (iter == m_pCameras.end())
        return E_FAIL;

    Safe_Release(iter->second);
    m_pCameras.erase(iter);
    return S_OK;
}

HRESULT CCameraManager::SetMainCamera(const WCHAR* szCameraTag, _float4x4* pPreCameraMatrix)
{
    if (nullptr != m_pMainCamera)
    {
        if (nullptr != pPreCameraMatrix)
        {
            memcpy(pPreCameraMatrix, m_pMainCamera->GetTransform()->Get_WorldMatrixPtr(), sizeof(_float4x4));
        }
        Safe_Release(m_pMainCamera);
    }

    m_pMainCamera = Find_Camera(szCameraTag);
    if (m_pMainCamera)
        Safe_AddRef(m_pMainCamera);
    else
        return E_FAIL;

    return S_OK;
}

CCamera* CCameraManager::GetCamrea(const WCHAR* szCameraTag)
{
    CCamera* pCamera = Find_Camera(szCameraTag);

    Safe_AddRef(pCamera);
    return pCamera;
}

CCamera* CCameraManager::GetMainCamera()
{
    if (nullptr == m_pMainCamera)
        return nullptr;

    Safe_AddRef(m_pMainCamera);
    return m_pMainCamera;
}

_bool CCameraManager::IsMainCamera(CCamera* pCamera)
{
    return m_pMainCamera == pCamera ? true : false;
}

_matrix CCameraManager::GetMainCameraWorldMatrix()
{
    if (nullptr == m_pMainCamera)
        return XMMatrixIdentity();

    return XMLoadFloat4x4(m_pMainCamera->GetTransform()->Get_WorldMatrixPtr());
}

const _float4x4* CCameraManager::GetMainCameraWorldMatrixPtr()
{
    if (nullptr == m_pMainCamera)
        return nullptr;

    return m_pMainCamera->GetTransform()->Get_WorldMatrixPtr();
}

_matrix CCameraManager::GetCameraWorldMatrix(const WCHAR* szCameraTag)
{
    CCamera* pCamera = Find_Camera(szCameraTag);
    if(nullptr == pCamera)
        return XMMatrixIdentity();

    return XMLoadFloat4x4(pCamera->GetTransform()->Get_WorldMatrixPtr());
}

const _float4x4* CCameraManager::GetCameraWorldMatrixPtr(const WCHAR* szCameraTag)
{
    CCamera* pCamera = Find_Camera(szCameraTag);
    if (nullptr == pCamera)
        return nullptr;

    return pCamera->GetTransform()->Get_WorldMatrixPtr();
}

void CCameraManager::Clear_Cameras()
{
    Safe_Release(m_pMainCamera);

    for (auto& iter : m_pCameras)
        Safe_Release(iter.second);
    m_pCameras.clear();
}

CCamera* CCameraManager::Find_Camera(const WCHAR* szCameraTag)
{
    auto iter = m_pCameras.find(szCameraTag);
    if (iter == m_pCameras.end())
        return nullptr;

    return iter->second;
}

CCameraManager* CCameraManager::Create()
{
    CCameraManager* pCameraManager = new CCameraManager();
    if (FAILED(pCameraManager->Initialize()))
    {
        Safe_Release(pCameraManager);
        MSG_BOX("Create Fail : Caemra Manager");
    }
    return pCameraManager;
}

void CCameraManager::Free()
{
    __super::Free();

    Safe_Release(m_pMainCamera);

    for (auto& iter : m_pCameras)
        Safe_Release(iter.second);

    m_pCameras.clear();
}
