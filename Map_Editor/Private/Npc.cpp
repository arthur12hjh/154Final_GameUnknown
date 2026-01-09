#include "pch.h"
#include "Npc.h"
#include "GameInstance.h"

CNpc::CNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDesertObject{ pDevice, pContext }
{
}

CNpc::CNpc(const CNpc& Prototype)
    : CDesertObject{ Prototype }
{
}

HRESULT CNpc::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNpc::Initialize(void* pArg)
{

    NPC_DESC* pDesc = static_cast<NPC_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (pDesc && pDesc->pComponentTag)
    {
        wcsncpy_s(m_ComponentTag, 256, pDesc->pComponentTag, _TRUNCATE);
    }

    if (FAILED(Ready_Components(m_ComponentTag)))
        return E_FAIL;

    m_iNpcID = pDesc->iNpcID;

    //SetCullingCollider(m_iObjectID);
    //_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    //m_pCullingCollider->UpdateColiision(worldMatrix);

    return S_OK;
}

void CNpc::Priority_Update(_float fTimeDelta)
{
}

void CNpc::Update(_float fTimeDelta)
{
    if (m_iNpcID == 1)
    {
        const _float4x4* pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();

        XMMATRIX WorldMatrix = XMLoadFloat4x4(pWorldMatrix);

        XMVECTOR Position = WorldMatrix.r[3];

        _float3 vPos;
        XMStoreFloat3(&vPos, Position);

        char szDebugString[256];
        sprintf_s(szDebugString, "Scarlet Position: X=%.2f, Y=%.2f, Z=%.2f\n", vPos.x, vPos.y, vPos.z);
        OutputDebugStringA(szDebugString);
    }
    /*SetCullingCollider(m_iObjectID);
    _matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    m_pCullingCollider->UpdateColiision(worldMatrix);*/
}

void CNpc::Late_Update(_float fTimeDelta)
{
    /*if (!m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
    {
        return;
    }*/

    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

    //m_pGameInstance->Add_RenderGroup(RENDER::OCCLUSION, this);

#ifdef _DEBUG
    //m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
}

HRESULT CNpc::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;


    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
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

HRESULT CNpc::Ready_Components(const _tchar* pComponentTag)
{
    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), pComponentTag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CNpc::Bind_ShaderResources()
{
    /*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

void CNpc::SetCullingCollider(_uint iObjectID)
{
    auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
    switch (iObjectID)
    {
    case 1: // Box_1A (1, 0.5, 0.8)
        pCullingCollider->SetCollision({ 0.f, 0.4f, 0.f }, {}, { 1.f, 0.5f, 0.8f });
        break;
    }
}

_uint CNpc::Object_Number(const _tchar* pComponentTag)
{
    if (pComponentTag == nullptr)
        return 0;

    const _tchar* pLastUnderscore = wcsrchr(pComponentTag, L'_');
    if (pLastUnderscore == nullptr || *(pLastUnderscore + 1) == L'\0')
        return 0;

    const _tchar* pSuffix = pLastUnderscore + 1;

    if (wcsstr(pComponentTag, TEXT("Garden"))) {
        if (!wcscmp(pSuffix, TEXT("1A"))) return 22;
        if (!wcscmp(pSuffix, TEXT("1B"))) return 23;
        if (!wcscmp(pSuffix, TEXT("1C"))) return 24;
    }

    return 0;
}

CNpc* CNpc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNpc* pInstance = new CNpc(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : pGraphic_Device");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CDesertObject* CNpc::Clone(void* pArg)
{
    CNpc* pInstance = new CNpc(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CNpc");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNpc::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
