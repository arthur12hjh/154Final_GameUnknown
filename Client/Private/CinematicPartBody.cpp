#include "pch.h"
#include "CinematicPartBody.h"

#include "Effect.h"
#include "Trail.h"
#include "TrailEffect.h"
#include "StringHelper.h"

#include "GameInstance.h"
#include "RimLight.h"

CCinematicPartBody::CCinematicPartBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CPartObject(pDevice, pContext)
{
}

CCinematicPartBody::CCinematicPartBody(const CCinematicPartBody& Prototype) :
    CPartObject(Prototype)
{
}

HRESULT CCinematicPartBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCinematicPartBody::Initialize(void* pArg)
{
    BODY_CINEMATIC_DESC* pDesc = static_cast<BODY_CINEMATIC_DESC*>(pArg);

    m_szModelTag = pDesc->szModelTag;
    m_pSocketMatrix = const_cast<_float4x4*>(pDesc->pSocketMatrix);

    m_bIsAnim = pDesc->isAnim;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (pDesc->isSetTransform)
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPartPosition), 1.f));
        m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vPartRotation.x),
            XMConvertToRadians(pDesc->vPartRotation.y),
            XMConvertToRadians(pDesc->vPartRotation.z));
        m_pTransformCom->Set_Scale(XMVectorSetW(XMLoadFloat3(&pDesc->vPartScale), 1.f));
	}

    m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::DIFFUSE, "g_DiffuseTexture");

    if(TEXT("Prototype_Component_Model_Cinematic_Dororong") != m_szModelTag)
        m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::NORMAL, "g_NormalTexture");

    //m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::EMISSIVE, "g_EmissiveTexture");
    m_pModelCom->Bind_MaterialTag(TEXTURE_TYPE::ORM, "g_ORMTexture");

    return S_OK;
}

void CCinematicPartBody::Priority_Update(_float fTimeDelta)
{
    //m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

    //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.5f, 1.f));

    Update_PreCombinedMatrix();
}

void CCinematicPartBody::Update(_float fTimeDelta)
{
    if (m_pSocketMatrix == nullptr)
    {
        XMStoreFloat4x4(&m_CombinedWorldMatrix,
            XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
    }
    else
    {
        _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

        for (size_t i = 0; i < 3; i++)
            SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

        XMStoreFloat4x4(&m_CombinedWorldMatrix,
            XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * SocketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
    }
}

void CCinematicPartBody::Late_Update(_float fTimeDelta)
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
}

HRESULT CCinematicPartBody::Render()
{
    if (m_bIsActive == FALSE)
        return S_OK;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
    
    if (m_bIsAnim)
    {
        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
                return E_FAIL;

            if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(9)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render(i)))
                return E_FAIL;
        }
    }
    else
    {
        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_AllMaterials(i, m_pShaderCom, 0)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render(i)))
                return E_FAIL;
        }
    }

    //m_pGameInstance->Render_Text(TEXT("KoPub"), m_szRotationAngle, _float2(g_iWinSizeX / 2 - 180, 0), XMVectorSet(1.f, 1.f, 1.f, 0.1f));

    return S_OK;
}

HRESULT CCinematicPartBody::Render_Shadow()
{
    if (m_bIsActive == FALSE)
        return S_OK;

    //Anim 객체든 아니든, 어차피 가까이서 보일거니까 다 캐스케이드로 처리함 수고
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_LightViewMatrix", D3DTS::VIEW)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Cascade(m_pShaderCom, "g_LightProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (m_bIsAnim)
        {
            if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(1)))
                return E_FAIL;
        }
        else
        {
            if (FAILED(m_pShaderCom->Begin(6)))
                return E_FAIL;
        }

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CCinematicPartBody::Render_MotionBlur()
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


    if (m_bIsAnim)
    {
        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(4)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render(i)))
                return E_FAIL;
        }
    }
    else
    {

    }

    return S_OK;
}

void CCinematicPartBody::Active_SFX(const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
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
        for (auto LineTrailEffect : m_pLineTrailEffects)
        {
            LineTrailEffect.first->bisPlay = false;
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

            SCARLET_TRAIL_DESC* ptrailDesc = new SCARLET_TRAIL_DESC;

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

        SCARLET_LINE_TRAIL_DESC* pLinetrailDesc = new SCARLET_LINE_TRAIL_DESC;

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

void CCinematicPartBody::Reset_SocketMatrix(_float4x4* pSocketMatrix)
{
	m_pSocketMatrix = pSocketMatrix;
}

void CCinematicPartBody::Stop_All_Effect()
{
    for (auto Effect : m_pEffects)
    {
        Effect.first->Stop();
    }
    for (auto TrailEffect : m_pTrailEffects)
    {
        TrailEffect.first->bisPlay = false;
    }
    for (auto LineTrailEffect : m_pLineTrailEffects)
    {
        LineTrailEffect.first->bisPlay = false;
    }
}

HRESULT CCinematicPartBody::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CCinematicPartBody::Ready_Components()
{
    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), m_szModelTag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if (m_bIsAnim)
    {
        /* Com_Shader */
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
            TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
            return E_FAIL;
    }
    else
    {
        /* Com_Shader */
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
            TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
            return E_FAIL;
    }

    return S_OK;
}

CCinematicPartBody* CCinematicPartBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCinematicPartBody* pCinematicPartBody = new CCinematicPartBody(pDevice, pContext);
    if (FAILED(pCinematicPartBody->Initialize_Prototype()))
    {
        Safe_Release(pCinematicPartBody);
        MSG_BOX("Create Fail : Cinematic Part Body");
    }
    return pCinematicPartBody;
}

CGameObject* CCinematicPartBody::Clone(void* pArg)
{
    CCinematicPartBody* pCinematicPartBody = new CCinematicPartBody(*this);
    if (FAILED(pCinematicPartBody->Initialize(pArg)))
    {
        Safe_Release(pCinematicPartBody);
        MSG_BOX("Clones Fail : Cinematic Part Body");
    }
    return pCinematicPartBody;
}

void CCinematicPartBody::Free()
{
    __super::Free();
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
    
}
