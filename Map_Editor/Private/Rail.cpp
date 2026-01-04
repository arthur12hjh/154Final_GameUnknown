#include "pch.h"
#include "Rail.h"
#include "GameInstance.h"

CRail::CRail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CDororong_Saber(pDevice, pContext)
{
}

CRail::CRail(const CRail& Prototype) :
    CDororong_Saber(Prototype)
{
}

HRESULT CRail::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRail::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CRail::Priority_Update(_float fTimeDelta)
{
}

void CRail::Update(_float fTimeDelta)
{
    m_fTimeAcc += fTimeDelta; 
}

void CRail::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);

#ifdef _DEBUG
    //m_pGameInstance->Add_DebugComponent(m_pCollider);
#endif

}

HRESULT CRail::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    /*if (FAILED(m_pShaderCom->Bind_RawValue("g_fColorWeight", &m_fColorWeight, sizeof(_float))))
        return E_FAIL;*/

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTimeAcc, sizeof(_float))))
        return E_FAIL;

    m_pShaderCom->Begin(1);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CRail::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxCube"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_VIBuffer_Cube"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CRail::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CRail* CRail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRail* pRail = new CRail(pDevice, pContext);
    if (FAILED(pRail->Initialize_Prototype()))
    {
        Safe_Release(pRail);
        MSG_BOX("Create Fail : Rail");
    }
    return pRail;
}

CGameObject* CRail::Clone(void* pArg)
{
    CRail* pRail = new CRail(*this);
    if (FAILED(pRail->Initialize(pArg)))
    {
        Safe_Release(pRail);
        MSG_BOX("Create Fail : Rail");
    }
    return pRail;
}

void CRail::Free()
{
    __super::Free();
}
