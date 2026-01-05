#include "pch.h"
#include "Rail.h"
#include "GameInstance.h"

CRail::CRail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CActor(pDevice, pContext)
{
}

CRail::CRail(const CRail& Prototype) :
    CActor(Prototype)
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
    _bool* pIsIdxFlags[3] = { &m_bIsIdx0, &m_bIsIdx1, &m_bIsIdx2 };
    _bool bIsAnyActive = false;

    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_J))
    {
        m_bIsColorArr[0] = true;
        m_bIsIdx0 = true;
        m_fTimeArr[0] = 0.f;
        m_fColorWeight = 0.f; // 만약 처음부터 다시 차오르게 하고 싶다면 주석 해제
    }

    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_K))
    {
        m_bIsColorArr[1] = true;
        m_bIsIdx1 = true;
        m_fTimeArr[1] = 0.f;
        m_fColorWeight = 0.f;
    }

    if (m_pGameInstance->KeyPressed(KEY_INPUT::KEYBOARD, DIK_L))
    {
        m_bIsColorArr[2] = true;
        m_bIsIdx2 = true;
        m_fTimeArr[2] = 0.f;
        m_fColorWeight = 0.f;
    }
    for (_uint i = 0; i < 3; i++)
    {
        if (m_bIsColorArr[i])
        {
            bIsAnyActive = true;
            m_fTimeArr[i] += fTimeDelta;

            if (m_fTimeArr[i] > 0.2f)
            {
                m_bIsColorArr[i] = false;
                *pIsIdxFlags[i] = false;
                m_fTimeArr[i] = 0.f;
            }
        }
    }

    if (bIsAnyActive)
    {
        m_fColorWeight += fTimeDelta * 2.f;

        if (m_fColorWeight > 1.f)
            m_fColorWeight = 1.f;
    }
    else
    {
        m_fColorWeight -= fTimeDelta * 2.f;
        if (m_fColorWeight < 0.f)
            m_fColorWeight = 0.f;
    }
}

void CRail::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
    //m_pGameInstance->Add_DebugComponent(m_pCollider);
#endif

}

HRESULT CRail::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fColorWeight", &m_fColorWeight, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsIdx0", &m_bIsIdx0, sizeof(_bool))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsIdx1", &m_bIsIdx1, sizeof(_bool))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsIdx2", &m_bIsIdx2, sizeof(_bool))))
        return E_FAIL;

    m_pShaderCom->Begin(1);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CRail::Ready_Components()
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

    Safe_Release(m_pVIBufferCom);
}
