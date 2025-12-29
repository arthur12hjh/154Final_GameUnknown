#include "pch.h"
#include "Virtual_Wall.h"
#include "GameInstance.h"

CVirtual_Wall::CVirtual_Wall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CGameObject(pDevice, pContext)
{
}

CVirtual_Wall::CVirtual_Wall(const CVirtual_Wall& Prototype) :
    CGameObject(Prototype)
{
}

HRESULT CVirtual_Wall::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVirtual_Wall::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CVirtual_Wall::Priority_Update(_float fTimeDelta)
{
}

void CVirtual_Wall::Update(_float fTimeDelta)
{
    _matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    //m_pCullingCollider->UpdateColiision(WorldMat);
    m_pCollider->UpdateColiision(WorldMat);
}

void CVirtual_Wall::Late_Update(_float fTimeDelta)
{
    /*if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
    {*/
    m_pGameInstance->Add_DebugComponent(m_pCollider);
    //m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
//}
}

HRESULT CVirtual_Wall::Render()
{
    return S_OK;
}

HRESULT CVirtual_Wall::Ready_Components()
{
    //COBBCollider::OBB_COLLIDER_DESC ObbDesc = {};
    //ObbDesc.vSize = {1.f, 1.f, 1.f};

    ///* Com_Shader */
    //if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::DESERT), TEXT("Prototype_Component_Collider_OBB"),
    //    TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider))))
    //    return E_FAIL;

    //static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision({}, {}, ObbDesc.vSize);

    /* Com_Collider_Sphere */
    /* Com_Collider_OBB */
    COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
    OBBDesc.vSize = _float3(1.f, 1.f, 1.f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y, 0.f);

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::VILLAGE), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pCollider), &OBBDesc)))
        return E_FAIL;

    return S_OK;
}

CVirtual_Wall* CVirtual_Wall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVirtual_Wall* pVirtual_Wall = new CVirtual_Wall(pDevice, pContext);
    if (FAILED(pVirtual_Wall->Initialize_Prototype()))
    {
        Safe_Release(pVirtual_Wall);
        MSG_BOX("Create Fail : Virtual_Wall");
    }
    return pVirtual_Wall;
}

CGameObject* CVirtual_Wall::Clone(void* pArg)
{
    CVirtual_Wall* pVirtual_Wall = new CVirtual_Wall(*this);
    if (FAILED(pVirtual_Wall->Initialize(pArg)))
    {
        Safe_Release(pVirtual_Wall);
        MSG_BOX("Create Fail : Virtual_Wall");
    }
    return pVirtual_Wall;
}

void CVirtual_Wall::Free()
{
    __super::Free();

    Safe_Release(m_pCollider);
}
