#include "pch.h"
#include "TestEveHead.h"

#include "GameInstance.h"

CTestEveHead::CTestEveHead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CTestEveHead::CTestEveHead(const CTestEveHead& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CTestEveHead::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTestEveHead::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CTestEveHead::Priority_Update(_float fTimeDelta)
{
}

void CTestEveHead::Update(_float fTimeDelta)
{
}

void CTestEveHead::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CTestEveHead::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
            return E_FAIL;
         if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
            return E_FAIL;

         if (strcmp(m_pModelCom->Get_MaterialName(m_pModelCom->Get_Mesh_MaterialIndex(i)), "Eye_TearLine") == 0)
         {
             if (FAILED(m_pShaderCom->Begin(4)))
                 return E_FAIL;
         }
         else
         {
             if (FAILED(m_pShaderCom->Begin(0)))
                 return E_FAIL;
         }
         if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CTestEveHead::Ready_Components()
{
    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_TestEveHead"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestEveHead::Bind_ShaderResources()
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

CTestEveHead* CTestEveHead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTestEveHead* pInstance = new CTestEveHead(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Create Failed : TestEveHead");
    }

    return pInstance;
}

CGameObject* CTestEveHead::Clone(void* pArg)
{
    CTestEveHead* pInstance = new CTestEveHead(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Clone Failed : TestEveHead");
    }

    return pInstance;
}

void CTestEveHead::Free()
{
    __super::Free();
    
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
