#include "pch.h"
#include "DororongBox.h"
#include "GameInstance.h"

CDororongBox::CDororongBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CActor(pDevice, pContext)
{
}

CDororongBox::CDororongBox(const CDororongBox& Prototype) :
    CActor(Prototype)
{
}

HRESULT CDororongBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDororongBox::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CDororongBox::Priority_Update(_float fTimeDelta)
{
}

void CDororongBox::Update(_float fTimeDelta)
{
    /*if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_J))
    {
        m_bIsColorChange = true;

    }
    if (m_bIsColorChange)
    {
        m_fColorWeight += fTimeDelta * 5.f;
        m_fTimeAcc += fTimeDelta;

        if (m_fColorWeight > 1.f)
            m_fColorWeight = 1.f;

        if (m_fTimeAcc > 0.2f)
        {
            m_bIsColorChange = false;
        }
    }
    else {
        m_fColorWeight -= fTimeDelta * 3.f;

        if (m_fColorWeight < 0.f)
            m_fColorWeight = 0.f;

        m_fTimeAcc = 0.f;
    }*/
}

void CDororongBox::Late_Update(_float fTimeDelta)
{
    /*if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
    {*/
    //m_pGameInstance->Add_DebugComponent(m_pCollider);
    //m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
    //}
    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
    //m_pGameInstance->Add_DebugComponent(m_pCollider);
#endif

}

HRESULT CDororongBox::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    /* COBBCollider* pObbCollider = static_cast<COBBCollider*>(m_pCollider);
     pObbCollider->Render_Face(_float4(0.f, 1.f, 0.f, 1.f));

     if (FAILED(m_pShaderCom->Begin(0)))
         return E_FAIL;*/

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fColorWeight", &m_fColorWeight, sizeof(_float))))
        return E_FAIL;

    m_pShaderCom->Begin(5);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CDororongBox::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxCube"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), TEXT("Prototype_Component_Texture_DororongMask"),
        TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CDororongBox::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_MaskTexture")))
        return E_FAIL;

    return S_OK;
}

CDororongBox* CDororongBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDororongBox* pDororongBox = new CDororongBox(pDevice, pContext);
    if (FAILED(pDororongBox->Initialize_Prototype()))
    {
        Safe_Release(pDororongBox);
        MSG_BOX("Create Fail : DororongBox");
    }
    return pDororongBox;
}

CGameObject* CDororongBox::Clone(void* pArg)
{
    CDororongBox* pDororongBox = new CDororongBox(*this);
    if (FAILED(pDororongBox->Initialize(pArg)))
    {
        Safe_Release(pDororongBox);
        MSG_BOX("Create Fail : DororongBox");
    }
    return pDororongBox;
}

void CDororongBox::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
}
