#include "pch.h"
#include "Pad.h"
#include "GameInstance.h"

CPad::CPad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

CPad::CPad(const CPad& Prototype) :
    CGameObject(Prototype)
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
    _matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    m_pCollider->UpdateColiision(WorldMat);
}

void CPad::Late_Update(_float fTimeDelta)
{
    /*if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
    {*/
    //m_pGameInstance->Add_DebugComponent(m_pCollider);
    //m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
    //}
}

HRESULT CPad::Render()
{
    COBBCollider* pObbCollider = static_cast<COBBCollider*>(m_pCollider);
    pObbCollider->Render_Face(_float4(0.f, 1.f, 0.f, 1.f));

    return S_OK;
}

HRESULT CPad::Ready_Components()
{
    ///* Com_Shader */
    //if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Collider_OBB"),
    //    TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider))))
    //    return E_FAIL;

    /* Com_Collider_OBB */
    COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
    OBBDesc.vSize = _float3(1.f, 1.f, 1.f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y, 0.f);

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pCollider), &OBBDesc)))
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

    Safe_Release(m_pCollider);
}
