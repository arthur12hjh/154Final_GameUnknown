#include "pch.h"
#include "StaticInteraction.h"

#include "GameInstance.h"
#include "UIBase.h"
#include "Interaction_Component.h"

CStaticInteraction::CStaticInteraction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CProb_Interaction(pDevice, pContext)
{
}

CStaticInteraction::CStaticInteraction(const CStaticInteraction& Prototype) :
    CProb_Interaction(Prototype)
{
}

HRESULT CStaticInteraction::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CStaticInteraction::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    ACTOR_DESC* pDesc = static_cast<ACTOR_DESC*>(pArg);
    if (FAILED(ADD_Components(*pDesc)))
        return E_FAIL;

    _matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    m_pCullingCollider->UpdateColiision(WorldMat);
    m_pRigidBody->Update_PxTransform(WorldMat);

    return S_OK;
}

void CStaticInteraction::Priority_Update(_float fTimeDelta)
{
}

void CStaticInteraction::Update(_float fTimeDelta)
{

}

void CStaticInteraction::Late_Update(_float fTimeDelta)
{
    if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
    {
        m_pInteractionCom->Update_Com();
#ifdef _DEBUG
        m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
        m_pGameInstance->Add_PhysxGeometry(m_pRigidBody->Get_PxRigidBody(), m_pRigidBody->Get_PxShape());
#endif

        m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
    }
}

HRESULT CStaticInteraction::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Begin(3)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CStaticInteraction::ADD_Components(const ACTOR_DESC& Desc)
{
    _float3 Com_Size = m_pTransformCom->Get_Scale();

    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), Desc.szVIBuffer_PrototypeName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Interaction */
    CInteraction_Component::INTERACTION_DESC InteractionDesc = {};
    InteractionDesc.vSize = Com_Size;
    InteractionDesc.BeginCallBackFunc = [&]() { this->Begin_OverlapCallBack(); };
    InteractionDesc.EndCallBackFunc = [&]() { this->End_OverlapCallBack(); };
    InteractionDesc.InteractionEvent = [&](CGameObject* pActionObject) { this->Excute_CallBack(pActionObject); };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Interaction"),
        TEXT("Com_Interaction"), reinterpret_cast<CComponent**>(&m_pInteractionCom), &InteractionDesc)))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    PxUserData tUserData;
    // 밀려야하는 애들은 이키워드로 세팅
    tUserData.szActorTag = TEXT("Static_Interaction");

    //리지드 바디 Desc 세팅. 머테리얼이랑 Mass, userdata, shape, type 부분 위주로 살펴보세요.
    CRigidBody::RIGIDBODY_DESC RigidBodyDesc;
    // 콜라이더 모양
    RigidBodyDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::BOX;

    // 충돌처리를 할지말지 
    // STATIC : 충돌하는데 가만히 있는 녀석
    // DYNAMIC : 충돌 
    // KINEMATIC : 충돌 X
    RigidBodyDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::KINEMATIC;

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

HRESULT CStaticInteraction::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CStaticInteraction::Begin_OverlapCallBack()
{
    if (m_pInteractionUI)
        m_pInteractionUI->SetVisibility(VISIBILITY::VISIBLE);

    m_pGameInstance->ADD_Interaction(m_pInteractionCom);
    m_bIsInteractionAble = true;
    return S_OK;
}

void CStaticInteraction::Excute_CallBack(CGameObject* pActionObject)
{
    if (INTERACTION_STATE::DEFAULT == m_eInterState)
        m_eInterState = INTERACTION_STATE::ACTIVE;
}

HRESULT CStaticInteraction::End_OverlapCallBack()
{
    if (m_pInteractionUI)
        m_pInteractionUI->SetVisibility(VISIBILITY::HIDDEN);

    m_pGameInstance->Remove_Interaction(m_pInteractionCom);
    m_bIsInteractionAble = false;
    return S_OK;
}

CStaticInteraction* CStaticInteraction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CStaticInteraction* pStaticInteraction = new CStaticInteraction(pDevice, pContext);
    if (FAILED(pStaticInteraction->Initialize_Prototype()))
    {
        Safe_Release(pStaticInteraction);
        MSG_BOX("Create Fail : Static Interaction");
    }
    return pStaticInteraction;
}

CGameObject* CStaticInteraction::Clone(void* pArg)
{
    CStaticInteraction* pStaticInteraction = new CStaticInteraction(*this);
    if (FAILED(pStaticInteraction->Initialize(pArg)))
    {
        Safe_Release(pStaticInteraction);
        MSG_BOX("Clone Fail : Static Interaction");
    }
    return pStaticInteraction;
}

void CStaticInteraction::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
}
