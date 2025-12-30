#include "pch.h"
#include "NayitbaPartBody.h"
#include "StringHelper.h"
#include "Effect.h"

#include "Trail.h"
#include "TrailEffect.h"
#include "RimLight.h"

#include "Texture.h"
#include "Nayitba.h"

#include "GameInstance.h"

CNayitbaPartBody::CNayitbaPartBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CPartObject(pDevice, pContext)
{
}

CNayitbaPartBody::CNayitbaPartBody(const CNayitbaPartBody& Prototype) :
    CPartObject(Prototype)
{
}

HRESULT CNayitbaPartBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNayitbaPartBody::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    NAYITBA_PART_BODY_DESC* pDesc = static_cast<NAYITBA_PART_BODY_DESC*>(pArg);
    if (FAILED(Ready_Components(*pDesc)))
        return E_FAIL;

    m_pModelCom->AddCount_PartialBone("Bip001-Spine2");
    m_pSpineMatrix = m_pModelCom->Get_BoneMatrixPtr("Bip001-Spine2");
    m_vDissolveRadius = 3.f;
    return S_OK;
}

void CNayitbaPartBody::Priority_Update(_float fTimeDelta)
{
    Update_PreCombinedMatrix();
}

void CNayitbaPartBody::Update(_float fTimeDelta)
{
    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
    
    //if (m_bIsChangeColorDissolve)
    //{
    //    m_fDeadTime += fTimeDelta * 5.f;
    //}

    if (m_isDeadEffect)
    {
        CNayitba* Naytiba = static_cast<CNayitba*>(m_pParent);
        if (0 >= m_fDeadTime && m_bisSetDeadEffect) {

            if (NAYTIBA_TYPE::ELITE == Naytiba->GetStaticMonsterData()->eNaytiba_Type) {
                CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
                EffectDesc.fRotationPerSec = 1.f;
                EffectDesc.fSpeedPerSec = 1.f;

                _float4x4 matTransform;
                XMStoreFloat4x4(&matTransform, XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Bip001-Spine2")) * XMLoadFloat4x4(&m_CombinedWorldMatrix));
                EffectDesc.pRootMatrix = nullptr;
                EffectDesc.pWorldMatrix = nullptr;
                EffectDesc.vPos = XMVectorSet(matTransform._41, matTransform._42 - 1.f, matTransform._43, matTransform._44);

                EffectDesc.fRot = _float3(0, XMConvertToRadians(55), 0);
                EffectDesc.fSize = 2.5f;
                EffectDesc.iFloor = 0;
                CEffect* pEffect = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Gigas_Dead"),
                    ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));
                matTransform._32 = 0;


                _vector vLook = XMVector3Normalize(XMVectorSet(matTransform._31, 0, matTransform._33, 0));
                _vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0, 1, 0, 0), vLook));
                _vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
                vLook *= EffectDesc.fSize;
                vRight *= EffectDesc.fSize;
                vUp *= EffectDesc.fSize;

                pEffect->GetTransform()->Set_State(STATE::RIGHT, vRight);
                pEffect->GetTransform()->Set_State(STATE::UP, vUp);
                pEffect->GetTransform()->Set_State(STATE::LOOK, vLook);
                pEffect->GetTransform()->Turn(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(28));
            }
            else {
                CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
                EffectDesc.fRotationPerSec = 1.f;
                EffectDesc.fSpeedPerSec = 1.f;

                _float4x4 matTransform;
                //XMStoreFloat4x4(&matTransform, XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Bip001-Spine2")) * XMLoadFloat4x4(&m_CombinedWorldMatrix));
                XMStoreFloat4x4(&matTransform, XMLoadFloat4x4(&m_CombinedWorldMatrix));
                EffectDesc.pRootMatrix = nullptr;
                EffectDesc.pWorldMatrix = nullptr;
                EffectDesc.vPos = XMVectorSet(matTransform._41, matTransform._42, matTransform._43, matTransform._44);

                EffectDesc.fRot = _float3(0, XMConvertToRadians(55), 0);
                EffectDesc.fSize = 1.f;
                EffectDesc.iFloor = 0;
                m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Ashes"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc);

            }

            m_bisSetDeadEffect = false;
        }
        if (NAYTIBA_TYPE::ELITE == Naytiba->GetStaticMonsterData()->eNaytiba_Type) {
            m_fDeadTime += fTimeDelta * 0.7f;
        }
        else {
            m_fDeadTime += fTimeDelta;
        }
        if (5 < m_fDeadTime) {
            m_pParent->Set_Dead(true);
        }
    }
}

void CNayitbaPartBody::Late_Update(_float fTimeDelta)
{
    m_fLineTime += fTimeDelta;
    for (auto TrailEffect : m_pTrailEffects)
    {
        if (nullptr == TrailEffect.first->pRootMatrix) {
            TrailEffect.first->pTrailEffect->Update_Trail(XMLoadFloat4x4(&m_CombinedWorldMatrix), fTimeDelta, TrailEffect.first->bisPlay);
        }
        else {
            TrailEffect.first->pTrailEffect->Update_Trail(XMLoadFloat4x4(TrailEffect.first->pRootMatrix) * XMLoadFloat4x4(&m_CombinedWorldMatrix), fTimeDelta, TrailEffect.first->bisPlay);
        }
    }
    for (auto LineTrailEffect : m_pLineTrailEffects)
    {
        if (nullptr == LineTrailEffect.first->pRootMatrix) {

            _matrix mat = XMLoadFloat4x4(&m_CombinedWorldMatrix);
            LineTrailEffect.first->pTrailEffect->Update_Trail(mat, LineTrailEffect.first->bisPlay ? fTimeDelta : fTimeDelta * 1.5f, LineTrailEffect.first->bisPlay);
        }
        else {
            _matrix mat = XMLoadFloat4x4(LineTrailEffect.first->pRootMatrix) * XMLoadFloat4x4(&m_CombinedWorldMatrix);
            LineTrailEffect.first->pTrailEffect->Update_Trail(mat, LineTrailEffect.first->bisPlay ? fTimeDelta : fTimeDelta * 1.5f, LineTrailEffect.first->bisPlay);
        }
    }
    
   //m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
   //m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CNayitbaPartBody::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;
    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORMTexture", aiTextureType_METALNESS, 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
            return E_FAIL;

        if (m_bIsChangeBodyColor)
        {
            if (FAILED(m_pShaderCom->Begin(8)))
                return E_FAIL;
        }
        else
        {
            if (0 < m_fDeadTime && m_isDeadEffect) {
                if (FAILED(m_pShaderCom->Begin(5)))
                    return E_FAIL;
            }
            else {
                if (FAILED(m_pShaderCom->Begin(0)))
                    return E_FAIL;
            }
        }

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }
    if (FAILED(End_ShaderResources()))
        return E_FAIL;
    return S_OK;
}

HRESULT CNayitbaPartBody::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_LightViewMatrix", D3DTS::VIEW)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_LightProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CNayitbaPartBody::Render_MotionBlur()
{
    /* 이전 프레임 월드매트릭스도 바인딩 */
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_PreWorldMatrix", &m_PreCombinedWorldMatrix)))
        return E_FAIL;

    /* 이전 뷰 매트릭스도 바인딩 */
    if (FAILED(m_pShaderCom->Bind_Matrix("g_PreViewMatrix", m_pGameInstance->Get_PreTransform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(4)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

void CNayitbaPartBody::Active_SFX(const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
    if (strObjectTag == TEXT("Play_Effect"))
    {
        CEffect* pEffect = nullptr;
        for (auto Effect : m_pEffects)
        {
            if (Effect.second == NotifyReference.iNumData01) {
                pEffect = Effect.first;
                break;
            }
        }
        
        if (nullptr == pEffect) {
            CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
            EffectDesc.fRotationPerSec = 1.f;
            EffectDesc.fSpeedPerSec = 1.f;
            EffectDesc.fSpeed = NotifyReference.fNumData01;
            if (NotifyReference.szSocketTag.compare("None") == 0)
            {
                EffectDesc.pRootMatrix = nullptr;
                EffectDesc.pWorldMatrix = nullptr;
            }
            else if (NotifyReference.szSocketTag.compare("Transform") == 0)
            {
                EffectDesc.pRootMatrix = &m_CombinedWorldMatrix;
                EffectDesc.pWorldMatrix = nullptr;
            }
            else
            {
                EffectDesc.pRootMatrix = m_pModelCom->Get_BoneMatrixPtr(NotifyReference.szSocketTag.c_str());
                EffectDesc.pWorldMatrix = &m_CombinedWorldMatrix;
            }

            EffectDesc.vPos = XMVectorSet(NotifyReference.vNotifyPosition.x, NotifyReference.vNotifyPosition.y, NotifyReference.vNotifyPosition.z, 1);
            EffectDesc.fRot = _float3(XMConvertToRadians(NotifyReference.vNotifyRotation.x), XMConvertToRadians(NotifyReference.vNotifyRotation.y), XMConvertToRadians(NotifyReference.vNotifyRotation.z));
            EffectDesc.fSize = NotifyReference.vNotifyScale.x;
            EffectDesc.iFloor = NotifyReference.iNumData02;
            _TCHAR szEffectTag[MAX_PATH];
            CStringHelper::ConvertUTFToWide(NotifyReference.szNotifyArg02.c_str(), szEffectTag);

            pEffect = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), szEffectTag,
                ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));

            Safe_AddRef(pEffect);
            m_pEffects.push_back({ pEffect, NotifyReference.iNumData01 });
        }
        pEffect->Play();
    }
    else if (strObjectTag == TEXT("Stop_Effect"))
    {
        CEffect* pEffect = nullptr;
        for (auto Effect : m_pEffects)
        {
            if (Effect.second == NotifyReference.iNumData01) {
                pEffect = Effect.first;
                break;
            }
        }
        if (nullptr != pEffect)
            pEffect->Stop();
    }
    else if (strObjectTag == TEXT("Stop_All_Effect"))
    {
        for (auto Effect : m_pEffects)
        {
            Effect.first->Stop();
        }
        for (auto TrailEffect : m_pTrailEffects)
        {
            TrailEffect.first->bisPlay = false;
        }
    }
    else if (strObjectTag == TEXT("Play_Trail"))
    {
        CTrailEffect* pTrailEffect = nullptr;
        for (auto TrailEffect : m_pTrailEffects)
        {
            if (TrailEffect.second == NotifyReference.iNumData01) {
                TrailEffect.first->bisPlay = true;
                return;
            }
        }

        if (nullptr == pTrailEffect) {

            NAYITBA_TRAIL_DESC* ptrailDesc = new NAYITBA_TRAIL_DESC;

            if (NotifyReference.szSocketTag.compare("Transform") != 0)
            {
                ptrailDesc->pRootMatrix = m_pModelCom->Get_BoneMatrixPtr(NotifyReference.szSocketTag.c_str());
            }
            ptrailDesc->bisPlay = true;

            CTrailEffect::TRAIL_DATA Traildesc{};
            Traildesc.vHigh = _float4(NotifyReference.vNotifyScale.x, NotifyReference.vNotifyScale.y, NotifyReference.vNotifyScale.z, 0.f);
            Traildesc.vLow = _float4(NotifyReference.vNotifyPosition.x, NotifyReference.vNotifyPosition.y, NotifyReference.vNotifyPosition.z, 0.f);
            Traildesc.bisLine = false;

            Traildesc.bisLong = NotifyReference.iNumData02 == 1;

            _TCHAR szEffectTag[MAX_PATH];
            CStringHelper::ConvertUTFToWide(NotifyReference.szNotifyArg02.c_str(), szEffectTag);

            pTrailEffect = static_cast<CTrailEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), szEffectTag,
                ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &Traildesc));
            ptrailDesc->pTrailEffect = pTrailEffect;
            Safe_AddRef(pTrailEffect);
            m_pTrailEffects.push_back({ ptrailDesc, NotifyReference.iNumData01 });
        }
    }
    else if (strObjectTag == TEXT("Stop_Trail"))
    {
        for (auto TrailEffect : m_pTrailEffects)
        {
            if (TrailEffect.second == NotifyReference.iNumData01) {
                TrailEffect.first->bisPlay = false;
                return;
            }
        }
    }

    else if (strObjectTag == TEXT("Play_LineTrail"))
    {
        CTrailEffect* pTrailEffect = nullptr;
        for (auto LineTrailEffect : m_pLineTrailEffects)
        {
            if (LineTrailEffect.second == NotifyReference.iNumData01) {
                LineTrailEffect.first->bisPlay = true;
                return;
            }
        }

        NAYITBA_LINE_TRAIL_DESC* pLinetrailDesc = new NAYITBA_LINE_TRAIL_DESC;

        if (NotifyReference.szSocketTag.compare("Transform") != 0)
        {
            pLinetrailDesc->pRootMatrix = m_pModelCom->Get_BoneMatrixPtr(NotifyReference.szSocketTag.c_str());
        }
        pLinetrailDesc->bisPlay = true;


        CTrailEffect::TRAIL_DATA Traildesc{};
        Traildesc.vHigh = _float4(NotifyReference.vNotifyScale.x, NotifyReference.vNotifyScale.y, NotifyReference.vNotifyScale.z, 0.f);
        Traildesc.vLow = _float4(NotifyReference.vNotifyPosition.x, NotifyReference.vNotifyPosition.y, NotifyReference.vNotifyPosition.z, 0.f);
        Traildesc.fSpeed = NotifyReference.fNumData01;
        Traildesc.fPow = NotifyReference.fNumData02;
        Traildesc.bisLine = true;

        _TCHAR szEffectTag[MAX_PATH];
        CStringHelper::ConvertUTFToWide(NotifyReference.szNotifyArg02.c_str(), szEffectTag);

        pTrailEffect = static_cast<CTrailEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), szEffectTag,
            ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &Traildesc));
        pLinetrailDesc->pTrailEffect = pTrailEffect;
        Safe_AddRef(pTrailEffect);
        m_pLineTrailEffects.push_back({ pLinetrailDesc, NotifyReference.iNumData01 });
    }
    else if (strObjectTag == TEXT("Stop_LineTrail"))
    {
        for (auto LineTrailEffect : m_pLineTrailEffects)
        {
            if (LineTrailEffect.second == NotifyReference.iNumData01) {
                LineTrailEffect.first->bisPlay = false;
                return;
            }
        }
        }
}

void CNayitbaPartBody::Play_DeadEffect()
{
    if (false == m_isDeadEffect)
    {
        m_bIsChangeColorDissolve = false;
        m_isDeadEffect = true;
        m_fDeadTime = 0.f;
    }
        
}

void CNayitbaPartBody::SetPart_BodyColor(_bool bIsEnable, _bool bIsDissolve, _float4 vColor)
{
    m_bIsChangeBodyColor = bIsEnable;
    /*m_bIsChangeColorDissolve = bIsDissolve;

    if (bIsDissolve)
    {
        _matrix WorldSpineMatrix = XMLoadFloat4x4(m_pSpineMatrix) * XMLoadFloat4x4(&m_CombinedWorldMatrix);
       XMStoreFloat4(&m_vColCenterPos, WorldSpineMatrix.r[3]);
       m_fDeadTime = 0.f;
       if (m_bIsDissolveFade != bIsDissolveFade)
       {
            m_fDeadTime = 0.f;
            m_bIsDissolveFade = bIsDissolveFade;
       }
    }*/

    m_vPatternColor = vColor;
    m_MonsterLimLightDesc.fRimLightIntensity = 10.f;
    m_MonsterLimLightDesc.fRimLightPower = 20.f;
    m_MonsterLimLightDesc.vRimLightColor = vColor;

    m_pRimLight->Set_RimLightDesc(m_MonsterLimLightDesc);
    m_pRimLight->Bind_RimLightShaderResources(m_pShaderCom, "g_vRimLightColor", "g_fRimLightPower", "g_fRimLightStrength", "g_vCamPosition");
}

HRESULT CNayitbaPartBody::Ready_Components(const NAYITBA_PART_BODY_DESC& pDesc)
{
    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), pDesc.szBodyModel,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Dissolve_bullet0.dds"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTexture))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_RimLight"),
        TEXT("Com_RimLight"), reinterpret_cast<CComponent**>(&m_pRimLight), &m_MonsterLimLightDesc)))
        return E_FAIL;

    ///* Com_Collider_Sphere */
    //CSphereCollider::SPHERE_COLLIDER_DESC		SphereDesc{};

    //SphereDesc.fRadius = 0.5f;
    //SphereDesc.vCenter = _float3(0.f, SphereDesc.fRadius, 0.f);

    //if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
    //	TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
    //	return E_FAIL;

    return S_OK;
}

HRESULT CNayitbaPartBody::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_IsPattern", &m_bIsChangeBodyColor, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vMeshColor", &m_vPatternColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_IsPatternNoise", &m_bIsChangeColorDissolve, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_IsFade", &m_bIsDissolveFade, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFadeRadius", &m_vDissolveRadius, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCenterPos", &m_vColCenterPos, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pTexture->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDeadTime", &m_fDeadTime, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

HRESULT CNayitbaPartBody::End_ShaderResources()
{
    _bool bIsFlag = false;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_IsPattern", &bIsFlag, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_IsPatternNoise", &bIsFlag, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_IsFade", &bIsFlag, sizeof(_bool))))
        return E_FAIL;
    return S_OK;
}

CNayitbaPartBody* CNayitbaPartBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNayitbaPartBody* pNayitbaPartBody = new CNayitbaPartBody(pDevice, pContext);
    if (FAILED(pNayitbaPartBody->Initialize_Prototype()))
    {
        Safe_Release(pNayitbaPartBody);
        MSG_BOX("Create Fail : Nayitba Part Body");
    }
    return pNayitbaPartBody;
}

CGameObject* CNayitbaPartBody::Clone(void* pArg)
{
    CNayitbaPartBody* pNayitbaPartBody = new CNayitbaPartBody(*this);
    if (FAILED(pNayitbaPartBody->Initialize(pArg)))
    {
        Safe_Release(pNayitbaPartBody);
        MSG_BOX("Clone Fail : Nayitba Part Body");
    }
    return pNayitbaPartBody;
}

void CNayitbaPartBody::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
    for (auto pEffect : m_pEffects)
        Safe_Release(pEffect.first);
    m_pEffects.clear();

    for (auto& pTrailEffect : m_pTrailEffects)
    {
        Safe_Release(pTrailEffect.first->pTrailEffect);
        Safe_Delete(pTrailEffect.first);
    }
     
    m_pTrailEffects.clear();


    for (auto& pLineTrailEffect : m_pLineTrailEffects)
    {
        Safe_Release(pLineTrailEffect.first->pTrailEffect);
        Safe_Delete(pLineTrailEffect.first);
    }

    m_pLineTrailEffects.clear();

    Safe_Release(m_pRimLight);
    Safe_Release(m_pTexture);
}
