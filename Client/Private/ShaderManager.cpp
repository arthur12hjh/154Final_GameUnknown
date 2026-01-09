#include "pch.h"
#include "ShaderManager.h"

#include "GameInstance.h"
#include "Camera.h"
//셰이더 매니저 통해서 리저브 해두면.. 꺼내쓰기 불편하지 않나?
#include "ReserveDeferred.h"
#include "TargetLight.h"
#include "GameManager.h"

#include "Player.h"
#include "Nayitba.h"
#include "Effect.h"
#include "CinematicObject.h"
#include "Moon.h"

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
    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F8))
    {
#pragma region DIRECTIONAL
        m_pDirectionalLightDesc = static_cast<LIGHT_DESC*>(m_pGameInstance->Get_Directional_Desc());
        m_pDirectionalLightDesc->vDiffuse = _float4(0.32f, 0.32f, 0.32f, 1.f);
#pragma endregion

#pragma region MB
        * m_pMotionBlurDesc->fCamBlurScale = { 0.05f };
        *m_pMotionBlurDesc->fObjectBlurScale = { 1.f };
        *m_pMotionBlurDesc->fBias = { 0.8f };
        *m_pMotionBlurDesc->iSampleCount = { 16 };
#pragma endregion

#pragma region FOG
        * m_pFogDesc->vFogColor = { 0.031f, 0.058f, 0.094f, 1.0000f };
        *m_pFogDesc->fFogStart = { 0.f };
        *m_pFogDesc->fFogEnd = { 285 };
        *m_pFogDesc->fFogPowerMin = { 0.5f };
        *m_pFogDesc->fFogPowerMax = { 1.f };
        *m_pFogDesc->fSkyboxFogPower = { 0.2f };
#pragma endregion

#pragma region SSAO
        * m_pSSAODesc->fRadiusMin = { 0.26f };
        *m_pSSAODesc->fRadiusMax = { 5.f };
        *m_pSSAODesc->fBiasMin = { 0.029f };
        *m_pSSAODesc->fBiasMax = { 0.059f };
        *m_pSSAODesc->fIntensity = { 0.68f };
#pragma endregion

#pragma region HDR
        * m_pHDRDesc->fHDRExposure = { 1.52f };
#pragma endregion

#pragma region VOLUMEFOG
        m_pVolumeFogDesc->pInscatterDesc->AsymmetryParameterG = 0.f;
        m_pVolumeFogDesc->pInscatterDesc->Density = 1.28f;
        m_pVolumeFogDesc->pInscatterDesc->Intensity = 0.08f;
        m_pVolumeFogDesc->pInscatterDesc->NoiseContrast = 0.572f;
        m_pVolumeFogDesc->pInscatterDesc->NoiseScale = 0.006f;
        m_pVolumeFogDesc->pInscatterDesc->NoiseStrength = 2.f;
        m_pVolumeFogDesc->pInscatterDesc->StartDistance = 36.f;
        m_pVolumeFogDesc->pInscatterDesc->FogAmbient = _float4(0.141f, 0.172f, 0.2f, 0.149f);
        m_pVolumeFogDesc->pInscatterDesc->LightColor = _float4(0.f, 0.f, 0.f, 1.f);
#pragma endregion
    }

    LEVEL eLevel = static_cast<LEVEL>(m_pGameInstance->GetCurrentLevelID());

    if(eLevel != LEVEL::END && eLevel != LEVEL::LOADING)
        Bind_CamInfo(eLevel);

    for (auto iter : m_ReserveDeferredShaders)
    {
        if (true == iter.second->Get_Active() && iter.second->Update(fTimeDelta)) {
            m_pGameInstance->Reserve_Deferred(iter.second);
        }
    }

    for (auto Pair : m_TargetLights)
    {
        Pair.second->Chase_Target();
    }

    for (auto iter : m_CinematicTargetLights)
    {
        iter->Chase_Target();
    }


    if (true == m_isScarletPhase2LerpTriggerOn)
        Load_Scarlet_Phase2_ShaderSettings(fTimeDelta);
    
    if (true == m_isScarletPhase2 && true == m_isScarletPatternFogActivated)
        Load_Scarlet_Pattern_Fog(fTimeDelta);
    else if (true == m_isScarletPhase2 && false == m_isScarletPatternFogActivated)
        Load_Scarlet_Normal_Fog(fTimeDelta);
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

    _bool bFlagInit = { false };


    for (_uint iLevelIdx = 0; iLevelIdx < ENUM_CLASS(LEVEL::END); ++iLevelIdx)
    {
        for (auto& iter : m_Shaders[iLevelIdx])
        {
            iter.second->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));
            iter.second->Bind_RawValue("g_fFar", &CamDesc.fFar, sizeof(_float));
            iter.second->Bind_RawValue("g_fNear", &CamDesc.fNear, sizeof(_float));
            iter.second->Bind_RawValue("g_fFOV", &CamDesc.fFov, sizeof(_float));
            //뎁스 B, W 채널에 들어갈 마스킹.
            iter.second->Bind_RawValue("g_IsMaskingDepthB", &bFlagInit, sizeof(_bool));
            iter.second->Bind_RawValue("g_IsMaskingDepthW", &bFlagInit, sizeof(_bool));
        }
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
        else if (1 == iIdx)
        {
            Load_TargetLights();
        }
        else if (2 == iIdx)
        {
            //2페이즈임을 체크. 
            m_isScarletPhase2 = true;
            m_isScarletPhase2LerpTriggerOn = true;
            m_fScarletPhase2LerpTimeAcc = 0.f;


            m_pPlayer->Set_DepthMaskingB(true);
            m_pScarlet->Set_DepthMaskingB(true);

            CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
            EffectDesc.fRotationPerSec = 1.f;
            EffectDesc.fSpeedPerSec = 1.f;
            EffectDesc.pWorldMatrix = {};
            EffectDesc.pRootMatrix = {};
            EffectDesc.pDir = {};
            EffectDesc.vPos = XMVectorSet(250.f, 25.f, 250.f, 1.f);
            EffectDesc.fRot = {};
            EffectDesc.fSize = 0.8f;
            EffectDesc.fSpeed = 1.05f;

            m_pMapEffect = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Sakura"),
                ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));
        }
        else if (3 == iIdx)
        {
            Load_Scarlet_BattleEnd_ShaderSettings();
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
    *m_pSSAODesc->fRadiusMin = { 0.173f };
    *m_pSSAODesc->fRadiusMax = { 0.141f };
    *m_pSSAODesc->fBiasMin = { 0.019f };
    *m_pSSAODesc->fBiasMax = { 0.62f };
    *m_pSSAODesc->fIntensity = { 2.32f };
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
#pragma region DIRECTIONAL
    m_pDirectionalLightDesc = static_cast<LIGHT_DESC*>(m_pGameInstance->Get_Directional_Desc());
    m_pDirectionalLightDesc->vDiffuse = _float4(0.32f, 0.32f, 0.32f, 1.f);
#pragma endregion

#pragma region MB
    *m_pMotionBlurDesc->fCamBlurScale = { 0.05f };
    *m_pMotionBlurDesc->fObjectBlurScale = { 1.f };
    *m_pMotionBlurDesc->fBias = { 0.8f };
    *m_pMotionBlurDesc->iSampleCount = { 16 };
#pragma endregion

#pragma region FOG
    *m_pFogDesc->vFogColor = { 0.031f, 0.058f, 0.094f, 1.0000f };
    *m_pFogDesc->fFogStart = { 0.f };
    *m_pFogDesc->fFogEnd = { 285 };
    *m_pFogDesc->fFogPowerMin = { 0.5f };
    *m_pFogDesc->fFogPowerMax = { 1.f };
    *m_pFogDesc->fSkyboxFogPower = { 0.2f };
#pragma endregion

#pragma region SSAO
    * m_pSSAODesc->fRadiusMin = { 0.26f };
    *m_pSSAODesc->fRadiusMax = { 5.f };
    *m_pSSAODesc->fBiasMin = { 0.029f };
    *m_pSSAODesc->fBiasMax = { 0.059f };
    *m_pSSAODesc->fIntensity = { 0.68f };
#pragma endregion

#pragma region HDR
    *m_pHDRDesc->fHDRExposure = { 1.52f };
#pragma endregion

#pragma region VOLUMEFOG
    m_pVolumeFogDesc->pInscatterDesc->AsymmetryParameterG = 0.f;
    m_pVolumeFogDesc->pInscatterDesc->Density = 1.28f;
    m_pVolumeFogDesc->pInscatterDesc->Intensity = 0.08f;
    m_pVolumeFogDesc->pInscatterDesc->NoiseContrast = 0.572f;
    m_pVolumeFogDesc->pInscatterDesc->NoiseScale = 0.006f;
    m_pVolumeFogDesc->pInscatterDesc->NoiseStrength = 2.f;
    m_pVolumeFogDesc->pInscatterDesc->StartDistance = 36.f;
    m_pVolumeFogDesc->pInscatterDesc->FogAmbient = _float4(0.141f, 0.172f, 0.2f, 0.149f);
    m_pVolumeFogDesc->pInscatterDesc->LightColor = _float4(0.f, 0.f, 0.f, 1.f);
#pragma endregion

#pragma region Cinematic
    map<_wstring, CCinematicObject*>* pCinematicObjects = CGameManager::GetInstance()->Get_CinematicObjectsMap();
    CTargetLight::TARGETLIGHT_DESC Desc{};
    Desc.tLightDesc.eType = LIGHT_TYPE::POINT;
    Desc.tLightDesc.vDiffuse = _float4(0.42f, 0.42f, 0.40f, 1.f);
    Desc.tLightDesc.vAmbient = _float4(0.4f, 0.2f, 0.2f, 1.f);
    Desc.tLightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
    Desc.tLightDesc.vPosition = _float4(20.f, 5.f, 20.f, 1.f);
    Desc.tLightDesc.fRange = 12.f;
    Desc.isCinematic = true;

    for (auto& Pair : *pCinematicObjects)
    {
        Desc.pTarget = Pair.second;
        CTargetLight* pTargetLight = CTargetLight::Create(&Desc);
        m_CinematicTargetLights.push_back(pTargetLight);
        pTargetLight->Set_Active(VISIBILITY::HIDDEN);
        pTargetLight->Set_Range(0.f);
    }
#pragma endregion
    
    //미리 포인터로 들고있게 하자
    m_pMoon = static_cast<CMoon*>(*m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::SCARLET), TEXT("Layer_Moon"))->begin());
    m_pMoon->Set_FactorColor(XMVectorSet(1.f, 1.f, 1.f, 1.f));

    m_ScarletPhase2LerpCache.vDirDiffuseStart = m_pDirectionalLightDesc->vDiffuse;

    m_ScarletPhase2LerpCache.vFogColorStart = *m_pFogDesc->vFogColor;
    m_ScarletPhase2LerpCache.fFogEndStart = *m_pFogDesc->fFogEnd;
    m_ScarletPhase2LerpCache.fFogPowerMinStart = *m_pFogDesc->fFogPowerMin;
    m_ScarletPhase2LerpCache.fFogPowerMaxStart = *m_pFogDesc->fFogPowerMax;
    m_ScarletPhase2LerpCache.fSkyboxFogPowerStart = *m_pFogDesc->fSkyboxFogPower;

    m_ScarletPhase2LerpCache.VolAsymmetryStart = m_pVolumeFogDesc->pInscatterDesc->AsymmetryParameterG;
    m_ScarletPhase2LerpCache.VolDensityStart = m_pVolumeFogDesc->pInscatterDesc->Density;
    m_ScarletPhase2LerpCache.VolIntensityStart = m_pVolumeFogDesc->pInscatterDesc->Intensity;
    m_ScarletPhase2LerpCache.VolNoiseContrastStart = m_pVolumeFogDesc->pInscatterDesc->NoiseContrast;
    m_ScarletPhase2LerpCache.VolNoiseScaleStart = m_pVolumeFogDesc->pInscatterDesc->NoiseScale;
    m_ScarletPhase2LerpCache.VolNoiseStrengthStart = m_pVolumeFogDesc->pInscatterDesc->NoiseStrength;
    m_ScarletPhase2LerpCache.VolStartDistanceStart = m_pVolumeFogDesc->pInscatterDesc->StartDistance;
    m_ScarletPhase2LerpCache.VolFogAmbientStart = m_pVolumeFogDesc->pInscatterDesc->FogAmbient;
    m_ScarletPhase2LerpCache.VolLightColorStart = m_pVolumeFogDesc->pInscatterDesc->LightColor;
}

void CShaderManager::Load_TargetLights()
{
#pragma region Player&Scarlet
    m_pScarlet = static_cast<CNaytiba*>(*m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::SCARLET), TEXT("Layer_Monster"))->begin());

    m_pPlayer = CGameManager::GetInstance()->GetGameCharacter();
    Safe_Release(m_pPlayer);

    CTargetLight::TARGETLIGHT_DESC Desc{};
    Desc.tLightDesc.eType = LIGHT_TYPE::POINT;
    Desc.tLightDesc.vDiffuse = _float4(0.42f, 0.42f, 0.40f, 1.f);
    Desc.tLightDesc.vAmbient = _float4(0.4f, 0.2f, 0.2f, 1.f);
    Desc.tLightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
    Desc.tLightDesc.vPosition = _float4(20.f, 5.f, 20.f, 1.f);
    Desc.tLightDesc.fRange = 12.f;
    Desc.pTarget = m_pPlayer;

    Safe_Release(Desc.pTarget);

    CTargetLight* pTargetLight = CTargetLight::Create(&Desc);
    m_TargetLights.emplace(TEXT("PlayerLight"), pTargetLight);

    Desc.pTarget = m_pScarlet;

    pTargetLight = CTargetLight::Create(&Desc);
    m_TargetLights.emplace(TEXT("ScarletLight"), pTargetLight);
#pragma endregion
}

void CShaderManager::Load_Scarlet_Phase2_ShaderSettings(_float fTimeDelta)
{
    m_fScarletPhase2LerpTimeAcc += fTimeDelta;
    _float fT = m_fScarletPhase2LerpTimeAcc / m_fScarletPhase2LerpTime;

    if (fT >= 1.f)
    {
        fT = 1.f;
        m_isScarletPhase2LerpTriggerOn = false; // 더 이상 업데이트 안 함
        m_fScarletPhase2LerpTimeAcc = 0.f;
    }


    if (m_pMoon)
    {
        _vector vStart = XMVectorSet(1.f, 1.f, 1.f, 1.f);          // 1페이즈 상수
        _vector vTarget = XMVectorSet(2.f, 0.7f, 0.7f, 1.f);        // 2페이즈 상수
        _vector vCol = XMVectorLerp(vStart, vTarget, fT);
        m_pMoon->Set_FactorColor(vCol);
    }

#pragma region DIRECTIONAL
    _float4 vDirTarget = _float4(0.654f, 0.f, 0.f, 1.f); // 기존 Phase2 상수
    XMStoreFloat4(&m_pDirectionalLightDesc->vDiffuse, XMVectorLerp(XMLoadFloat4(&m_ScarletPhase2LerpCache.vDirDiffuseStart), 
        XMLoadFloat4(&vDirTarget), fT));
#pragma endregion

#pragma region FOG
    _float4 vFogTarget = _float4(0.f, 0.f, 0.f, 1.f);
    _float  fFogEndTar = 285.f;
    _float  fFogPowMinTar = 0.5f;
    _float  fFogPowMaxTar = 1.f;
    _float  fSkyFogTar = 0.22f;

    XMStoreFloat4(&(*m_pFogDesc->vFogColor), XMVectorLerp( 
        XMLoadFloat4(&m_ScarletPhase2LerpCache.vFogColorStart), XMLoadFloat4(&vFogTarget), fT));

    *m_pFogDesc->fFogEnd = Lerp<_float>(m_ScarletPhase2LerpCache.fFogEndStart, fFogEndTar, fT);
    *m_pFogDesc->fFogPowerMin = Lerp<_float>(m_ScarletPhase2LerpCache.fFogPowerMinStart, fFogPowMinTar, fT);
    *m_pFogDesc->fFogPowerMax = Lerp<_float>(m_ScarletPhase2LerpCache.fFogPowerMaxStart, fFogPowMaxTar, fT);
    *m_pFogDesc->fSkyboxFogPower = Lerp<_float>(m_ScarletPhase2LerpCache.fSkyboxFogPowerStart, fSkyFogTar, fT);
#pragma endregion

#pragma region VOLUMEFOG
    //  phase 1 setting
    //  m_pVolumeFogDesc->pInscatterDesc->AsymmetryParameterG = 0.f;
    //  m_pVolumeFogDesc->pInscatterDesc->Density = 1.28f;
    //  m_pVolumeFogDesc->pInscatterDesc->Intensity = 0.08f;
    //  m_pVolumeFogDesc->pInscatterDesc->NoiseContrast = 0.572f;
    //  m_pVolumeFogDesc->pInscatterDesc->NoiseScale = 0.006f;
    //  m_pVolumeFogDesc->pInscatterDesc->NoiseStrength = 2.f;
    //  m_pVolumeFogDesc->pInscatterDesc->StartDistance = 36.f;
    //  m_pVolumeFogDesc->pInscatterDesc->FogAmbient = _float4(0.141f, 0.172f, 0.2f, 0.149f);
    //  m_pVolumeFogDesc->pInscatterDesc->LightColor = _float4(0.f, 0.f, 0.f, 1.f);

    auto* pInscatter = m_pVolumeFogDesc->pInscatterDesc;

    _float  AsymTar = 0.f;
    _float  DenTar = 1.28f;
    _float  IntTar = 0.11f;
    _float  NoiseConTar = 0.572f;
    _float  NoiseScaleTar = 0.004f;
    _float  NoiseStrTar = 1.67f;
    _float  StartDistTar = 5.8f;

    _float4 FogAmbTar = _float4(0.094f, 0.094f, 0.094f, 0.149f);
    _float4 LightColTar = _float4(0.129f, 0.129f, 0.129f, 0.129f);

    pInscatter->AsymmetryParameterG = Lerp<_float>(m_ScarletPhase2LerpCache.VolAsymmetryStart, AsymTar, fT);
    pInscatter->Density = Lerp<_float>(m_ScarletPhase2LerpCache.VolDensityStart, DenTar, fT);
    pInscatter->Intensity = Lerp<_float>(m_ScarletPhase2LerpCache.VolIntensityStart, IntTar, fT);
    pInscatter->NoiseContrast = Lerp<_float>(m_ScarletPhase2LerpCache.VolNoiseContrastStart, NoiseConTar, fT);
    pInscatter->NoiseScale = Lerp<_float>(m_ScarletPhase2LerpCache.VolNoiseScaleStart, NoiseScaleTar, fT);
    pInscatter->NoiseStrength = Lerp<_float>(m_ScarletPhase2LerpCache.VolNoiseStrengthStart, NoiseStrTar, fT);
    pInscatter->StartDistance = Lerp<_float>(m_ScarletPhase2LerpCache.VolStartDistanceStart, StartDistTar, fT);

    XMStoreFloat4(&pInscatter->FogAmbient, XMVectorLerp(
            XMLoadFloat4(&m_ScarletPhase2LerpCache.VolFogAmbientStart), XMLoadFloat4(&FogAmbTar), fT));

    XMStoreFloat4(&pInscatter->LightColor,XMVectorLerp(
            XMLoadFloat4(&m_ScarletPhase2LerpCache.VolLightColorStart), XMLoadFloat4(&LightColTar), fT));
#pragma endregion
}

/* 스칼렛 사라지면서 터진다.. */
void CShaderManager::Load_Scarlet_BattleEnd_ShaderSettings()
{
    auto iter = m_TargetLights.find(TEXT("ScarletLight"));
    Safe_Release(iter->second); 
    m_TargetLights.erase(iter);
    
    if (nullptr != m_pMapEffect)
    {
        m_pMapEffect->End();
        m_pMapEffect = nullptr;
    }
}

void CShaderManager::Load_Scarlet_Pattern_Fog(_float fTimeDelta)
{
    m_fScarletPatternFogTimeAcc += fTimeDelta;
    _float fT = m_fScarletPatternFogTimeAcc / m_fScarletPatternFogLerpTime;

    if (fT >= 1.f)
        fT = 1.f;

#pragma region FOG
    _float4 vFogTarget = _float4(0.f, 0.f, 0.f, 1.f);
    _float  fFogEndTar = 56.f;
    _float  fFogPowMinTar = 0.f;
    _float  fFogPowMaxTar = 1.f;
    _float  fSkyFogTar = 0.22f;

    XMStoreFloat4(&(*m_pFogDesc->vFogColor), XMVectorLerp(
        XMLoadFloat4(&m_ScarletPhase2LerpCache.vFogColorStart), XMLoadFloat4(&vFogTarget), fT));

    *m_pFogDesc->fFogEnd = Lerp<_float>(m_ScarletPhase2LerpCache.fFogEndStart, fFogEndTar, fT);
    *m_pFogDesc->fFogPowerMin = Lerp<_float>(m_ScarletPhase2LerpCache.fFogPowerMinStart, fFogPowMinTar, fT);
    *m_pFogDesc->fFogPowerMax = Lerp<_float>(m_ScarletPhase2LerpCache.fFogPowerMaxStart, fFogPowMaxTar, fT);
    *m_pFogDesc->fSkyboxFogPower = Lerp<_float>(m_ScarletPhase2LerpCache.fSkyboxFogPowerStart, fSkyFogTar, fT);
#pragma endregion
}

void CShaderManager::Load_Scarlet_Normal_Fog(_float fTimeDelta)
{
    m_fScarletPatternFogTimeAcc += fTimeDelta;
    _float fT = m_fScarletPatternFogTimeAcc / m_fScarletPatternFogLerpTime;

    if (fT >= 1.f)
        fT = 1.f;

#pragma region FOG
    _float4 vFogTarget = _float4(0.f, 0.f, 0.f, 1.f);
    _float  fFogEndTar = 285.f;
    _float  fFogPowMinTar = 0.5f;
    _float  fFogPowMaxTar = 1.f;
    _float  fSkyFogTar = 0.22f;

    XMStoreFloat4(&(*m_pFogDesc->vFogColor), XMVectorLerp(
        XMLoadFloat4(&m_ScarletPhase2LerpCache.vFogColorStart), XMLoadFloat4(&vFogTarget), fT));

    *m_pFogDesc->fFogEnd = Lerp<_float>(56.f, fFogEndTar, fT);
    *m_pFogDesc->fFogPowerMin = Lerp<_float>(0.f, fFogPowMinTar, fT);
    *m_pFogDesc->fFogPowerMax = Lerp<_float>(1.f, fFogPowMaxTar, fT);
    *m_pFogDesc->fSkyboxFogPower = Lerp<_float>(0.22f, fSkyFogTar, fT);
#pragma endregion
}

void CShaderManager::Set_CinematicLights(_uint iFlag)
{
    VISIBILITY bFlag = {};
    VISIBILITY bObjFlag = {};
    switch (iFlag)
    {
    case 1:
        bFlag = VISIBILITY::VISIBLE;
        bObjFlag = VISIBILITY::HIDDEN;
        break;
    case 0:
        bFlag = VISIBILITY::HIDDEN;
        bObjFlag = VISIBILITY::VISIBLE;
        break;
    }

    for (auto& iter : m_CinematicTargetLights)
    {
        iter->Set_Active(bFlag);
        iter->Set_Range(12.f);
    }

    for (auto& Pair : m_TargetLights)
        Pair.second->Set_Active(bObjFlag);

    map<_wstring, CCinematicObject*>* pCinematicObjects = CGameManager::GetInstance()->Get_CinematicObjectsMap();
    for (auto& Pair : *pCinematicObjects)
    {
        Pair.second->Set_DepthMaskingB(iFlag == 1 ? true : false);
    }
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

    for (auto& iter : m_CinematicTargetLights)
        Safe_Release(iter);
    m_CinematicTargetLights.clear();

    for (auto& iter : m_ReserveDeferredShaders)
    {
        Safe_Release(iter.second);
    }
    m_ReserveDeferredShaders.clear();

    for (auto& Pair : m_TargetLights)
    {
        Safe_Release(Pair.second);
    }
    m_TargetLights.clear();
}
