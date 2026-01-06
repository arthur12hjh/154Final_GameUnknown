#include "pch.h"
#include "Pad.h"
#include "GameInstance.h"

CPad::CPad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CActor(pDevice, pContext)
{
}

CPad::CPad(const CPad& Prototype) :
    CActor(Prototype)
{
}

HRESULT CPad::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPad::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CPad::Priority_Update(_float fTimeDelta)
{
}

void CPad::Update(_float fTimeDelta)
{
    /*_matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    m_pCollider->UpdateColiision(WorldMat);*/

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

void CPad::Late_Update(_float fTimeDelta)
{
    /*if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
    {*/
    //m_pGameInstance->Add_DebugComponent(m_pCollider);
    //m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
    //}
    m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);

#ifdef _DEBUG
    //m_pGameInstance->Add_DebugComponent(m_pCollider);
#endif

}

HRESULT CPad::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    /* COBBCollider* pObbCollider = static_cast<COBBCollider*>(m_pCollider);
     pObbCollider->Render_Face(_float4(0.f, 1.f, 0.f, 1.f));

     if (FAILED(m_pShaderCom->Begin(0)))
         return E_FAIL;*/

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fColorWeight", &m_fColorWeight, sizeof(_float))))
        return E_FAIL;

    /*m_pShaderCom->Begin(1);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();*/

    m_pShaderCom->Begin(3);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CPad::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxCube"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CPad::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CPad* CPad::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPad* pPad = new CPad(pDevice, pContext);
    if (FAILED(pPad->Initialize_Prototype()))
    {
        Safe_Release(pPad);
        MSG_BOX("Create Fail : Pad");
    }
    return pPad;
}

CGameObject* CPad::Clone(void* pArg)
{
    CPad* pPad = new CPad(*this);
    if (FAILED(pPad->Initialize(pArg)))
    {
        Safe_Release(pPad);
        MSG_BOX("Create Fail : Pad");
    }
    return pPad;
}

void CPad::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
}
