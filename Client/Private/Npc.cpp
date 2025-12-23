#include "pch.h"
#include "Npc.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "StringHelper.h"
#include "Interaction_Component.h"

#include "AIController.h"
#include "NpcBody.h"
#include "NpcFace.h"

CNpc::CNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CCharacter(pDevice, pContext)
{
}

CNpc::CNpc(const CNpc& Prototype) :
    CCharacter(Prototype)
{
}

HRESULT CNpc::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNpc::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    NPC_DESC* pNpcDesc = static_cast<NPC_DESC*>(pArg);
    m_NpcDesc = m_pGameManager->Find_NpcData(pNpcDesc->iNpcID);

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CNpc::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
    
    m_pAIController->Priority_Update(fTimeDelta);
}

void CNpc::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    m_pCullingCollider->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
    m_pAIController->Update(fTimeDelta);
}

void CNpc::Late_Update(_float fTimeDelta)
{
    if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
    {
        m_pAIController->Late_Update(fTimeDelta);

        if (m_bIsActive == TRUE)
        {
            m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

            _float fCamDist = XMVectorGetX(
                XMVector3Length(
                    m_pTransformCom->Get_State(STATE::POSITION) - XMLoadFloat4(m_pGameInstance->Get_CamPosition()
                    )));

            if (fCamDist < 100.f)
            {
                m_pGameInstance->Add_RenderGroup(RENDER::MOTIONBLUR, this);
                m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
            }

        }
        __super::Late_Update(fTimeDelta);

#ifdef _DEBUG
        m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif // _DEBUG
    }
}

HRESULT CNpc::Render()
{
    for (auto& pPartObject : m_PartObjects)
        pPartObject.second->Render();

    return S_OK;
}

HRESULT CNpc::Render_Shadow()
{
    for (auto& pPartObject : m_PartObjects)
        pPartObject.second->Render_Shadow();

    return S_OK;
}

HRESULT CNpc::Ready_PartObjects()
{
    WCHAR	ModelProtoType[MAX_PATH] = {};
    CStringHelper::ConvertUTFToWide(m_NpcDesc->szPrototypeModel, ModelProtoType);

    CNpcBody::NPC_PART_BODY_DESC BodyDesc = { };
    BodyDesc.pParentTransform = m_pTransformCom;
    BodyDesc.vScale = { 1.f, 1.f, 1.f };
    BodyDesc.szBodyModel = ModelProtoType;
    BodyDesc.fSpeedPerSec = 5.f;
    if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_NpcBody"), TEXT("Part_Body"), &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CNpc::Ready_Components()
{
    _float3 Com_Size = { 3.f, 3.f, 3.f };

    WCHAR	NpcController[MAX_PATH] = {};
    CStringHelper::ConvertUTFToWide(m_NpcDesc->szAIController, NpcController);

    CAIController::GAMEOBJECT_DESC ControllerDesc = { };
    ControllerDesc.pParent = this;

    CBase* pInstnace = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), NpcController, &ControllerDesc);
    if (nullptr == pInstnace)
        return E_FAIL;
    m_pAIController = static_cast<CAIController*>(pInstnace);

    /* Com_Interaction */
    CInteraction_Component::INTERACTION_DESC InteractionDesc = {};
    InteractionDesc.vSize = Com_Size;
    InteractionDesc.BeginCallBackFunc = [&]() { this->Begin_Interaction(); };
    InteractionDesc.EndCallBackFunc = [&]() { this->End_Interaction(); };
    InteractionDesc.InteractionEvent = [&](_float fTimeDelta, CGameObject* pActionObject) { Excute_Interaction(fTimeDelta, pActionObject); };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Interaction"),
        TEXT("Com_Interaction"), reinterpret_cast<CComponent**>(&m_pInteractionCom), &InteractionDesc)))
        return E_FAIL;

    m_pInteractionCom->SetInteractionHitType(HIT_TYPE::INTERACTION);
    m_pInteractionCom->ADD_InteractionIgnoreObject(HIT_TYPE::NPC);


    PxUserData tUserData;
    tUserData.szActorTag = TEXT("KIMETIC_Actor2");

    //리지드 바디 Desc 세팅. 머테리얼이랑 Mass, userdata, shape, type 부분 위주로 살펴보세요.
    CRigidBody::RIGIDBODY_DESC RigidBodyDesc;
    // 콜라이더 모양
    RigidBodyDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::BOX;

    // 충돌처리를 할지말지 
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
    static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision({}, {}, InteractionDesc.vSize);
    return S_OK;
}

void CNpc::Begin_Interaction()
{
}

void CNpc::Excute_Interaction(_float fTimeDelta, CGameObject* pActionObject)
{

}

void CNpc::End_Interaction()
{
}

CNpc* CNpc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNpc* pNpc = new CNpc(pDevice, pContext);
    if (FAILED(pNpc->Initialize_Prototype()))
    {
        Safe_Release(pNpc);
        MSG_BOX("Create Fail : Npc");
    }
    return pNpc;
}

CGameObject* CNpc::Clone(void* pArg)
{
    CNpc* pNpc = new CNpc(*this);
    if (FAILED(pNpc->Initialize(pArg)))
    {
        Safe_Release(pNpc);
        MSG_BOX("Clone Fail : Npc");
    }
    return pNpc;
}

void CNpc::Free()
{
    __super::Free();

    Safe_Release(m_pRigidBody);
    Safe_Release(m_pAIController);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pInteractionCom);
}
