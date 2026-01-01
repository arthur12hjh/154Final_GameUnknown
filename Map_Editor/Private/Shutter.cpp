#include "pch.h"
#include "Shutter.h"
#include "GameInstance.h"

CShutter::CShutter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDesertObject{ pDevice, pContext }
{
}

CShutter::CShutter(const CShutter& Prototype)
    : CDesertObject{ Prototype }
{
}

HRESULT CShutter::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CShutter::Initialize(void* pArg)
{

    DESERT_OBJECT_DESC* pDesc = static_cast<DESERT_OBJECT_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (pDesc && pDesc->pComponentTag)
    {
        wcsncpy_s(m_ComponentTag, 256, pDesc->pComponentTag, _TRUNCATE);
    }

    if (FAILED(Ready_Components(m_ComponentTag)))
        return E_FAIL;

    //_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    //m_pCullingCollider->UpdateColiision(worldMatrix);

    return S_OK;
}

void CShutter::Priority_Update(_float fTimeDelta)
{
}

void CShutter::Update(_float fTimeDelta)
{
    _matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    m_pCullingCollider->UpdateColiision(worldMatrix);
}

void CShutter::Late_Update(_float fTimeDelta)
{
    /*if (!m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
    {
        return;
    }*/

    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

    //m_pGameInstance->Add_RenderGroup(RENDER::OCCLUSION, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
}

HRESULT CShutter::Render()
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

HRESULT CShutter::Ready_Components(const _tchar* pComponentTag)
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

HRESULT CShutter::Bind_ShaderResources()
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

CShutter* CShutter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CShutter* pInstance = new CShutter(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : pGraphic_Device");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CDesertObject* CShutter::Clone(void* pArg)
{
    CShutter* pInstance = new CShutter(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CShutter");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CShutter::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
