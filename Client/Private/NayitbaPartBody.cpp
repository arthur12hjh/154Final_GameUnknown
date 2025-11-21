#include "pch.h"
#include "NayitbaPartBody.h"

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

    return S_OK;
}

void CNayitbaPartBody::Priority_Update(_float fTimeDelta)
{
}

void CNayitbaPartBody::Update(_float fTimeDelta)
{
    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

    //m_pColliderCom->UpdateColiision(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CNayitbaPartBody::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

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
        if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
            return E_FAIL;


        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;


        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CNayitbaPartBody::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }
    return S_OK;
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
}
