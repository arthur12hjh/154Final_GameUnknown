#include "pch.h"
#include "Npc.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "StringHelper.h"
#include "Interaction_Component.h"

#include "PlayerCCTHitReporter.h"
#include "PlayerBehaviorCallback.h"
#include "PlayerCCTQueryFilterCallback.h"

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
    
    m_pCCT->Update_PrePxPosition(m_pTransformCom);
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
        m_pInteractionCom->Update_Com(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
        m_pAIController->Late_Update(fTimeDelta);
        m_pCCT->Update_PxPosition(fTimeDelta, m_pTransformCom);

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


    /* Com_CCT */
    CCharacterController::CCT_DESC Desc;
    PxUserData tUserData;
    tUserData.szActorTag = TEXT("Player_CCT");

    Desc.eCharacterControllerType = CCharacterController::CCT_SHAPE::CAPSULE;
    Desc.tUserData = tUserData;
    //캡슐 컨트롤러에서 x는 구 성분 y는 기둥 성분
    Desc.vSize = { m_NpcDesc->vExtents.x, m_NpcDesc->vExtents.y, 0.f };
    XMStoreFloat4(&Desc.vStartPos, m_pTransformCom->Get_State(STATE::POSITION));
    Desc.vMaterial = _float3(0.5f, 0.5f, 0.f);
    Desc.pHitReporter = CPlayerCCTHitReporter::Create();
    Desc.pBehaviorCallback = CPlayerBehaviorCallback::Create();
    Desc.pQueryFilterCallback = CPlayerCCTQueryFilterCallback::Create();
    Desc.iCollisionGroup = PHYSX_CCT;
    Desc.fStepOffset = 0.05f;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterController"),
        TEXT("Com_CCT"), reinterpret_cast<CComponent**>(&m_pCCT), &Desc)))
        return E_FAIL;

    m_pGameInstance->Add_CCT_ToPhysx(this, m_pCCT);
    static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision({}, {}, InteractionDesc.vSize);
    return S_OK;
}

void CNpc::Begin_Interaction()
{
    m_pGameInstance->ADD_Interaction(m_pInteractionCom);
}

void CNpc::Excute_Interaction(_float fTimeDelta, CGameObject* pActionObject)
{

}

void CNpc::End_Interaction()
{
    m_pGameInstance->Remove_Interaction(m_pInteractionCom);
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

    Safe_Release(m_pAIController);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pInteractionCom);
}
