#include "pch.h"
#include "BeatSaberCharacterBody.h"

#include "GameInstance.h"
#include "RimLight.h"

CBeatSaberCharacterBody::CBeatSaberCharacterBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CPartObject(pDevice, pContext)
{
}

CBeatSaberCharacterBody::CBeatSaberCharacterBody(const CBeatSaberCharacterBody& Prototype) :
    CPartObject(Prototype)
{
}

HRESULT CBeatSaberCharacterBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBeatSaberCharacterBody::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CBeatSaberCharacterBody::Priority_Update(_float fTimeDelta)
{
    Update_PreCombinedMatrix();
}

void CBeatSaberCharacterBody::Update(_float fTimeDelta)
{
    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

    //m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);
}

void CBeatSaberCharacterBody::Late_Update(_float fTimeDelta)
{
}

HRESULT CBeatSaberCharacterBody::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;
    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORMTexture", aiTextureType_METALNESS, 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(11)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CBeatSaberCharacterBody::Render_Shadow()
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

HRESULT CBeatSaberCharacterBody::Render_MotionBlur()
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

HRESULT CBeatSaberCharacterBody::Bind_ShaderResources()
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

HRESULT CBeatSaberCharacterBody::Ready_Components()
{
    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::BEATSABER_GAME), TEXT("Prototype_Component_Model_Dororong"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_RimLight"),
        TEXT("Com_RimLight"), reinterpret_cast<CComponent**>(&m_pRimLight), &m_LimLightDesc)))
        return E_FAIL;

    return S_OK;
}

CBeatSaberCharacterBody* CBeatSaberCharacterBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBeatSaberCharacterBody* pBeatSaberCharacterBody = new CBeatSaberCharacterBody(pDevice, pContext);
    if (FAILED(pBeatSaberCharacterBody->Initialize_Prototype()))
    {
        Safe_Release(pBeatSaberCharacterBody);
        MSG_BOX("Create Fail : Beat Saber Character Body");
    }
    return pBeatSaberCharacterBody;
}

CGameObject* CBeatSaberCharacterBody::Clone(void* pArg)
{
    CBeatSaberCharacterBody* pBeatSaberCharacterBody = new CBeatSaberCharacterBody(*this);
    if (FAILED(pBeatSaberCharacterBody->Initialize(pArg)))
    {
        Safe_Release(pBeatSaberCharacterBody);
        MSG_BOX("Clones Fail : Beat Saber Character Body");
    }
    return pBeatSaberCharacterBody;
}

void CBeatSaberCharacterBody::Free()
{
    __super::Free();

    Safe_Release(m_pRimLight);
}
