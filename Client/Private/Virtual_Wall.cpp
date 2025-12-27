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

    m_pCullingCollider->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    return S_OK;
}

void CVirtual_Wall::Priority_Update(_float fTimeDelta)
{
}

void CVirtual_Wall::Update(_float fTimeDelta)
{
}

void CVirtual_Wall::Late_Update(_float fTimeDelta)
{
    //m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
    //m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
}

HRESULT CVirtual_Wall::Render()
{
    return S_OK;
}

HRESULT CVirtual_Wall::Ready_Components()
{
    _float3 Com_Size = m_pTransformCom->Get_Scale();

    PxUserData tUserData;
    tUserData.szActorTag = TEXT("KIMETIC_Actor2");

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
    RigidBodyDesc.vSize = Com_Size;
    RigidBodyDesc.fMass = { 0.3f };

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
