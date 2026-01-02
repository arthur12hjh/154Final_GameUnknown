#include "pch.h"
#include "SpawnBox.h"
#include "GameInstance.h"

CSpawnBox::CSpawnBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDesertObject{ pDevice, pContext }
{
}

CSpawnBox::CSpawnBox(const CSpawnBox& Prototype)
    : CDesertObject{ Prototype }
{
}

HRESULT CSpawnBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSpawnBox::Initialize(void* pArg)
{

    MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (pDesc && pDesc->pComponentTag)
    {
        wcsncpy_s(m_ComponentTag, 256, pDesc->pComponentTag, _TRUNCATE);
    }

    if (FAILED(Ready_Components(m_ComponentTag)))
        return E_FAIL;

    m_iMonsterID = pDesc->iMonsterID;

    //SetCullingCollider(m_iObjectID);
    //_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    //m_pCullingCollider->UpdateColiision(worldMatrix);

    return S_OK;
}

void CSpawnBox::Priority_Update(_float fTimeDelta)
{
}

void CSpawnBox::Update(_float fTimeDelta)
{
    /*SetCullingCollider(m_iObjectID);
    _matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    m_pCullingCollider->UpdateColiision(worldMatrix);*/
}

void CSpawnBox::Late_Update(_float fTimeDelta)
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

HRESULT CSpawnBox::Render()
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

HRESULT CSpawnBox::Ready_Components(const _tchar* pComponentTag)
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

HRESULT CSpawnBox::Bind_ShaderResources()
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

CSpawnBox* CSpawnBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSpawnBox* pInstance = new CSpawnBox(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : pGraphic_Device");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CDesertObject* CSpawnBox::Clone(void* pArg)
{
    CSpawnBox* pInstance = new CSpawnBox(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CSpawnBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSpawnBox::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
