#include "pch.h"
#include "MousePointer.h"

#include "GameInstance.h"

CMousePointer::CMousePointer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CUIObject(pDevice, pContext)
{
}

CMousePointer::CMousePointer(const CMousePointer& Prototype) :
    CUIObject(Prototype)
{
}

HRESULT CMousePointer::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMousePointer::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_iZOrder = INF_MAX;
    return S_OK;
}

void CMousePointer::Priority_Update(_float fTimeDelta)
{
    POINT MousePoint = m_pGameInstance->GetMousePoint();
    MousePoint.x += m_pTransformCom->Get_Scale().x * 0.25f;
    MousePoint.y += m_pTransformCom->Get_Scale().y * 0.3f;
    ComputeTransform(XMVectorSet(MousePoint.x, MousePoint.y, 0.f, 1.f));
} 

void CMousePointer::Update(_float fTimeDelta)
{

}

void CMousePointer::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CMousePointer::Render()
{
    if(FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMousePointer::Ready_Components()
{
    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Point"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Mouse"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_Mouse"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMousePointer::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", GetCombinedMatrixPtr())))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->GetIdentityMatrixPtr())))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", ENUM_CLASS(m_eMosueScale))))
        return E_FAIL;

    return S_OK;
}

CMousePointer* CMousePointer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMousePointer* pMousePointer = new CMousePointer(pDevice, pContext);
    if (FAILED(pMousePointer->Initialize_Prototype()))
    {
        Safe_Release(pMousePointer);
        MSG_BOX("Create Fail : Mouse Pointer");
    }
    return pMousePointer;
}

CGameObject* CMousePointer::Clone(void* pArg)
{
    CMousePointer* pMousePointer = new CMousePointer(*this);
    if (FAILED(pMousePointer->Initialize(pArg)))
    {
        Safe_Release(pMousePointer);
        MSG_BOX("Clone Fail : Mouse Pointer");
    }
    return pMousePointer;
}

void CMousePointer::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
