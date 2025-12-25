#include "pch.h"
#include "ShaderManager.h"

#include "GameInstance.h"
#include "Camera.h"
//셰이더 매니저 통해서 리저브 해두면.. 꺼내쓰기 불편하지 않나?
#include "ReserveDeferred.h"

CShaderManager::CShaderManager()
    : m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CShaderManager::Initialize()
{
    m_Shaders = new map<_wstring, class CShader*>[ENUM_CLASS(LEVEL::END)];
    return S_OK;
}

HRESULT CShaderManager::Add_Shader(LEVEL eLevelID, const _wstring& strShaderTag, CShader* pShader)
{
    m_Shaders[ENUM_CLASS(eLevelID)].emplace(strShaderTag, pShader);
    Safe_AddRef(pShader);

    return S_OK;
}

CShader* CShaderManager::Get_Shader(LEVEL eLevelID, const _wstring& strShaderTag)
{
    auto iter = m_Shaders[ENUM_CLASS(eLevelID)].find(strShaderTag);

    if (iter == m_Shaders[ENUM_CLASS(eLevelID)].end())
        return nullptr;

    return iter->second;
}

/* 등록된 후처리들은 일단 다 처리해준다. */
void CShaderManager::Update(_float fTimeDelta)
{
    LEVEL eLevel = static_cast<LEVEL>(m_pGameInstance->GetCurrentLevelID());

    if(eLevel != LEVEL::END && eLevel != LEVEL::LOADING)
        Bind_CamInfo(eLevel);

    for (auto iter : m_ReserveDeferredShaders)
    {
        if(true == iter.second->Get_Active())
            m_pGameInstance->Reserve_Deferred(iter.second);
    }
}

void CShaderManager::Clear(LEVEL eLevelID)
{
    for (auto& iter : m_Shaders[ENUM_CLASS(eLevelID)])
    {
        Safe_Release(iter.second);
    }
	m_Shaders[ENUM_CLASS(eLevelID)].clear();
}

HRESULT CShaderManager::Bind_CamInfo(LEVEL eLevelID)
{
    //파이프라인에서 이번 프레임 받아옴
    CAMERA_INFO CamDesc = m_pGameInstance->Get_CurrentCamInfo();

    for (auto& iter : m_Shaders[ENUM_CLASS(eLevelID)])
    {
		iter.second->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));
		iter.second->Bind_RawValue("g_fFar", &CamDesc.fFar, sizeof(_float));
        iter.second->Bind_RawValue("g_fNear", &CamDesc.fNear, sizeof(_float));
        iter.second->Bind_RawValue("g_fFOV", &CamDesc.fFov, sizeof(_float));
    }

    return S_OK;
}

HRESULT CShaderManager::Add_ReserveDeferred(const _wstring& strReserveDeferredTag, CReserveDeferred* pReserveDeferred)
{
    if (nullptr == pReserveDeferred)
        return E_FAIL;

    auto iter = m_ReserveDeferredShaders.find(strReserveDeferredTag);
    if (iter != m_ReserveDeferredShaders.end())
        return E_FAIL;

    m_ReserveDeferredShaders.emplace(make_pair(strReserveDeferredTag, pReserveDeferred));

    return S_OK;
}

void CShaderManager::Set_Active_ReserveDeferred(const _wstring& strReserveDeferredTag, _bool bFlag)
{
    auto iter = m_ReserveDeferredShaders.find(strReserveDeferredTag);
    if (iter == m_ReserveDeferredShaders.end())
        return;

    iter->second->Set_Active(bFlag);
}

void CShaderManager::Set_Desc_ReserveDeferred(const _wstring& strReserveDeferredTag, void* pArg)
{
    auto iter = m_ReserveDeferredShaders.find(strReserveDeferredTag);
    if (iter == m_ReserveDeferredShaders.end())
        return;

    iter->second->Set_Desc(pArg);
}

CShaderManager* CShaderManager::Create()
{
	CShaderManager* pInstance = new CShaderManager();

    if(FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CShaderManager");
        Safe_Release(pInstance);
	}   

    return pInstance;
}

void CShaderManager::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);

	for (_uint i = 0; i < ENUM_CLASS(LEVEL::END); ++i)
    {
        for (auto& iter : m_Shaders[i])
        {
            Safe_Release(iter.second);
        }
        m_Shaders[i].clear();
    }
    Safe_Delete_Array(m_Shaders);

    for (auto& iter : m_ReserveDeferredShaders)
    {
        Safe_Release(iter.second);
    }
    m_ReserveDeferredShaders.clear();
}
