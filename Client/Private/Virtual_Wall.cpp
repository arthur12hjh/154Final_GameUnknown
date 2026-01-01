#include "pch.h"
#include "Virtual_Wall.h"
#include "GameInstance.h"

CVirtual_Wall::CVirtual_Wall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CActor(pDevice, pContext)
{
}

CVirtual_Wall::CVirtual_Wall(const CVirtual_Wall& Prototype) :
    CActor(Prototype)
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

    /*if (FAILED(Ready_Components()))
        return E_FAIL;*/

    //_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    //m_pCollider->UpdateColiision(worldMatrix);

    return S_OK;
}

void CVirtual_Wall::Priority_Update(_float fTimeDelta)
{
}

void CVirtual_Wall::Update(_float fTimeDelta)
{
    if (m_bIsReady == false)
    {
        if (FAILED(Ready_Components()))
            return;

        m_bIsReady = true;
    }
    m_pCollider->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));


}

void CVirtual_Wall::Late_Update(_float fTimeDelta)
{
    //m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pCollider);
#endif
}

HRESULT CVirtual_Wall::Render()
{
    return S_OK;
}

HRESULT CVirtual_Wall::Ready_Components()
{
    COBBCollider::OBB_COLLIDER_DESC		OBBDesc{};
    OBBDesc.vSize = _float3(1.f, 1.f, 1.f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y, 0.f);

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pCollider), &OBBDesc)))
        return E_FAIL;

    PxUserData tUserData;
    tUserData.szActorTag = TEXT("Wall_Actor2");

    //리지드 바디 Desc 세팅. 머테리얼이랑 Mass, userdata, shape, type 부분 위주로 살펴보세요.
    CRigidBody::RIGIDBODY_DESC RigidBodyDesc;
    // 콜라이더 모양
    RigidBodyDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::BOX;

    // 충돌처리를 할지말지 
    // DYNAMIC : 충돌 
    // KINEMATIC : 충돌 X
    RigidBodyDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::STATIC;

    RigidBodyDesc.StartWorldMatrix = *m_pTransformCom->Get_WorldMatrixPtr();
    RigidBodyDesc.tUserData = tUserData;
    RigidBodyDesc.vMaterial = _float3(0.5f, 0.5f, 0.3f);
    RigidBodyDesc.vSize = m_pTransformCom->Get_Scale();
    RigidBodyDesc.fMass = { 0.3f };
    _float3 vRigidSize = RigidBodyDesc.vSize;
    RigidBodyDesc.vSize = _float3(vRigidSize.x * 1.9f, vRigidSize.y * 16.f, vRigidSize.z * 4.f);

    /* Com_RigidBody */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
        TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
        return E_FAIL;

    // 리지드 바디 세팅 끝났으면 Physx 매니저에 집어넣는 과정도 있어야돼요.
    // 없으면 충돌 안됨
    m_pGameInstance->Add_RigidBody_ToPhysx(this, m_pRigidBody);

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
