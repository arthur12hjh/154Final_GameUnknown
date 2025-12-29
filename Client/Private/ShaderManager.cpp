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

    m_pMotionBlurDesc = static_cast<MOTIONBLUR_DESC*>(m_pGameInstance->Get_MotionBlur_Desc());
    m_pFogDesc = static_cast<FOG_DESC*>(m_pGameInstance->Get_Fog_Desc());
    m_pBloomDesc = static_cast<BLOOM_DESC*>(m_pGameInstance->Get_Bloom_Desc());
    m_pDoFDesc = static_cast<DOF_DESC*>(m_pGameInstance->Get_DoF_Desc());
    m_pSSAODesc = static_cast<SSAO_DESC*>(m_pGameInstance->Get_SSAO_Desc());
    m_pVolumetricDesc = static_cast<VOLUMETRIC_DESC*>(m_pGameInstance->Get_Volumetric_Desc());
    m_pHDRDesc = static_cast<HDR_DESC*>(m_pGameInstance->Get_HDR_Desc());
    m_pDirectionalLightDesc = static_cast<LIGHT_DESC*>(m_pGameInstance->Get_Directional_Desc());
    m_pVolumeFogDesc = static_cast<VOLUMEFOG_DESC*>(m_pGameInstance->Get_VolumeFog_Desc());

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

void CShaderManager::Change_ShaderSetting(LEVEL eLevelID, _uint iIdx)
{
    switch (eLevelID)
    {
    case LEVEL::GAMEPLAY:
        Load_Desert_ShaderSettings();
        break;

    case LEVEL::SCARLET:
        if (0 == iIdx)
        {
            Load_Scarlet_ShaderSettings();
        }
        else
        {
            Load_Scarlet_Phase2_ShaderSettings();
        }
        break;
    default:
        break;
    }
}

void CShaderManager::Load_Desert_ShaderSettings()
{
#pragma region MB
    *m_pMotionBlurDesc->fCamBlurScale = { 0.05f };
    *m_pMotionBlurDesc->fObjectBlurScale = { 1.f };
    *m_pMotionBlurDesc->fBias = { 0.8f };
    *m_pMotionBlurDesc->iSampleCount = { 16 };
#pragma endregion

#pragma region FOG
    *m_pFogDesc->vFogColor = { 1.0f, 0.89f, 0.70f, 1.f };
    *m_pFogDesc->fFogStart = { 0.f };
    *m_pFogDesc->fFogEnd = { 500.f };
    *m_pFogDesc->fFogPowerMin = { 0.5f };
    *m_pFogDesc->fFogPowerMax = { 1.f };
    *m_pFogDesc->fSkyboxFogPower = { 0.77f };
#pragma endregion

#pragma region SSAO
    *m_pSSAODesc->fRadiusMin = { 0.26f };
    *m_pSSAODesc->fRadiusMax = { 5.f };
    *m_pSSAODesc->fBiasMin = { 0.029f };
    *m_pSSAODesc->fBiasMax = { 0.059f };
    *m_pSSAODesc->fIntensity = { 5.f };
#pragma endregion

#pragma region HDR
    *m_pHDRDesc->fHDRExposure = { 1.52f };
#pragma endregion

#pragma region VOLUMEFOG
    m_pVolumeFogDesc->pInscatterDesc->Intensity = 0.f;
#pragma endregion
}

void CShaderManager::Load_Scarlet_ShaderSettings()
{
#pragma region MB
    * m_pMotionBlurDesc->fCamBlurScale = { 0.05f };
    *m_pMotionBlurDesc->fObjectBlurScale = { 1.f };
    *m_pMotionBlurDesc->fBias = { 0.8f };
    *m_pMotionBlurDesc->iSampleCount = { 16 };
#pragma endregion

#pragma region FOG
    *m_pFogDesc->vFogColor = { 0.1922f, 0.2235f, 0.2706f, 1.f };
    *m_pFogDesc->fFogStart = { 0.f };
    *m_pFogDesc->fFogEnd = { 285 };
    *m_pFogDesc->fFogPowerMin = { 0.5f };
    *m_pFogDesc->fFogPowerMax = { 1.f };
    *m_pFogDesc->fSkyboxFogPower = { 0.14f };
#pragma endregion

#pragma region SSAO
    * m_pSSAODesc->fRadiusMin = { 0.26f };
    *m_pSSAODesc->fRadiusMax = { 5.f };
    *m_pSSAODesc->fBiasMin = { 0.029f };
    *m_pSSAODesc->fBiasMax = { 0.059f };
    *m_pSSAODesc->fIntensity = { 1.4f };
#pragma endregion

#pragma region HDR
    *m_pHDRDesc->fHDRExposure = { 1.52f };
#pragma endregion

#pragma region VOLUMEFOG
    m_pVolumeFogDesc->pInscatterDesc->Intensity = 0.f;
    m_pVolumeFogDesc->pInscatterDesc->AsymmetryParameterG = -1.f;
    m_pVolumeFogDesc->pInscatterDesc->Density = 0.58f;
    m_pVolumeFogDesc->pInscatterDesc->Intensity = 0.1f;
    m_pVolumeFogDesc->pInscatterDesc->NoiseContrast = 0.572f;
    m_pVolumeFogDesc->pInscatterDesc->NoiseScale = 0.006f;
    m_pVolumeFogDesc->pInscatterDesc->NoiseStrength = 2.f;
    m_pVolumeFogDesc->pInscatterDesc->StartDistance = 122.6f;
    m_pVolumeFogDesc->pInscatterDesc->LightColor = _float4(0.f, 0.f, 0.f, 1.f);
#pragma endregion

    m_pDirectionalLightDesc = static_cast<LIGHT_DESC*>(m_pGameInstance->Get_Directional_Desc());
    m_pDirectionalLightDesc->vDiffuse = _float4(0.274f, 0.274f, 0.274f, 1.f);
}

void CShaderManager::Load_Scarlet_Phase2_ShaderSettings()
{
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
