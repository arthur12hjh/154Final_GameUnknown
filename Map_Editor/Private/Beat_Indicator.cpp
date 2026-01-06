#include "pch.h"
#include "Beat_Indicator.h"
#include "GameInstance.h"

CBeat_Indicator::CBeat_Indicator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CDororong_Saber(pDevice, pContext)
{
}

CBeat_Indicator::CBeat_Indicator(const CBeat_Indicator& Prototype) :
    CDororong_Saber(Prototype)
{
}

HRESULT CBeat_Indicator::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBeat_Indicator::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fBounceTimer = 0.f;

    return S_OK;
}

void CBeat_Indicator::Priority_Update(_float fTimeDelta)
{
}

void CBeat_Indicator::Update(_float fTimeDelta)
{
    /*_matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    m_pCollider->UpdateColiision(WorldMat);*/

    _float fBounceTime = 0.4f;
    m_fBounceTimer += fTimeDelta;

    /*if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_J) || 
        m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_K) || 
        m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_L))*/
    if(m_fBounceTimer > fBounceTime)
    {
        m_bIsColorChange = true;
        //m_fColorWeight = 0.f;
        m_fRandom = (rand() / (float)RAND_MAX) * 0.4f;
        m_fBounceTimer = 0.f;

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
        m_fColorWeight -= fTimeDelta * 8.f;

        if (m_fColorWeight < 0.f)
            m_fColorWeight = 0.f;

        m_fTimeAcc = 0.f;
    }

}

void CBeat_Indicator::Late_Update(_float fTimeDelta)
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

HRESULT CBeat_Indicator::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fColorWeight", &m_fColorWeight, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fBeatRandom", &m_fRandom, sizeof(_float))))
        return E_FAIL;

    /*m_pShaderCom->Begin(1);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();*/

    m_pShaderCom->Begin(4);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CBeat_Indicator::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Shader_VtxCube"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_VIBuffer_Cube"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Collider_OBB */
    COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
    OBBDesc.vSize = _float3(1.f, 1.f, 1.f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pCollider), &OBBDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBeat_Indicator::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CBeat_Indicator* CBeat_Indicator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBeat_Indicator* pBeat_Indicator = new CBeat_Indicator(pDevice, pContext);
    if (FAILED(pBeat_Indicator->Initialize_Prototype()))
    {
        Safe_Release(pBeat_Indicator);
        MSG_BOX("Create Fail : Beat_Indicator");
    }
    return pBeat_Indicator;
}

CGameObject* CBeat_Indicator::Clone(void* pArg)
{
    CBeat_Indicator* pBeat_Indicator = new CBeat_Indicator(*this);
    if (FAILED(pBeat_Indicator->Initialize(pArg)))
    {
        Safe_Release(pBeat_Indicator);
        MSG_BOX("Create Fail : Beat_Indicator");
    }
    return pBeat_Indicator;
}

void CBeat_Indicator::Free()
{
    __super::Free();
}
