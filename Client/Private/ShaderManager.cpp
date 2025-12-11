#include "pch.h"
#include "ShaderManager.h"

#include "GameInstance.h"
#include "Camera.h"

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

void CShaderManager::Update(_float fTimeDelta)
{
    LEVEL eLevel = static_cast<LEVEL>(m_pGameInstance->GetCurrentLevelID());

    if(eLevel != LEVEL::END && eLevel != LEVEL::LOADING)
        Bind_CamInfo(eLevel);
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
}
