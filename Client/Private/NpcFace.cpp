#include "pch.h"
#include "NpcFace.h"

#include "GameInstance.h"

CNpcFace::CNpcFace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CPartObject(pDevice, pContext)
{
}

CNpcFace::CNpcFace(const CNpcFace& Prototype) :
    CPartObject(Prototype)
{
}

HRESULT CNpcFace::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNpcFace::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CNpcFace::Priority_Update(_float fTimeDelta)
{
}

void CNpcFace::Update(_float fTimeDelta)
{
}

void CNpcFace::Late_Update(_float fTimeDelta)
{
}

HRESULT CNpcFace::Render()
{
    return S_OK;
}

HRESULT CNpcFace::Render_Shadow()
{
    return S_OK;
}

HRESULT CNpcFace::Ready_Components()
{
    return S_OK;
}

HRESULT CNpcFace::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CNpcFace* CNpcFace::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNpcFace* pNpcFace = new CNpcFace(pDevice, pContext);
    if (FAILED(pNpcFace->Initialize_Prototype()))
    {
        Safe_Release(pNpcFace);
        MSG_BOX("Create Fail : Npc Face");
    }
    return pNpcFace;
}

CGameObject* CNpcFace::Clone(void* pArg)
{
    CNpcFace* pNpcFace = new CNpcFace(*this);
    if (FAILED(pNpcFace->Initialize(pArg)))
    {
        Safe_Release(pNpcFace);
        MSG_BOX("Clone Fail : Npc Face");
    }
    return pNpcFace;
}

void CNpcFace::Free()
{
    __super::Free();
}
