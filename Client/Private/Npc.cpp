#include "pch.h"
#include "Npc.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "StringHelper.h"
#include "InteractionBinder.h"

#include "PlayerCCTHitReporter.h"
#include "PlayerBehaviorCallback.h"
#include "PlayerCCTQueryFilterCallback.h"

#include "AIController.h"
#include "DropComponent.h"

#pragma region Part Model
#include "NpcBody.h"
#include "Npc_LeftWeaponPart.h"
#include "Npc_RightWeaponPart.h"
#include "NpcFace.h"
#pragma endregion

#include "UIHUD.h"
#include "UIScript.h"
#include "Item.h"

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

    if (5 == m_NpcDesc->iNpcID)
        m_pDropCom->DropRewardItem();

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

    if (m_pGameInstance->isIn_DistanceFrustum(m_pTransformCom->Get_State(STATE::POSITION), 150.f))
    {
        if (INTERACTION_STATE::ACTIVE == m_pInteractionCom->Get_InterState())
        {
            CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

            if (!pHUD)
            {
                Safe_Release(pHUD);
                return;
            }

            if (!dynamic_cast<CUIScript*>(pHUD->Get_UIObject(TEXT("Layer_Script"), TEXT("UI_Scripts")))->Get_Has_Script_Desc())
            {
                if (m_NpcDesc->iNpcID == 4) // 상점 npc
                {
                    pHUD->Open_Shop();
                }
                m_pInteractionCom->Set_InterState(INTERACTION_STATE::DEFAULT);
            }

            Safe_Release(pHUD);
        }
        //else
        //{
        //    auto pPlayerDesc = m_pGameManager->Get_PlayerDesc();
        //    if (PLAYER_MODE::BATTLE == pPlayerDesc->ePlayerMode || PLAYER_MODE::IDLE == pPlayerDesc->ePlayerMode)
        //    {
        //        //if (INTERACTION_STATE::LOCK == m_pInteractionCom->Get_InterState())
        //        {
        //            // 나중에 여기서 조건 체크하세요
        //            //m_pInteractionCom->Set_InterState(INTERACTION_STATE::DEFAULT);
        //        }
        //    }
        //}
    }

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
                //m_pGameInstance->Add_RenderGroup(RENDER::MOTIONBLUR, this);
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

    Import_ModelPtr();
    if (strcmp("None", m_NpcDesc->szFaceName))
    {
        CNpcFace::FACE_DESC FaceDesc = { };
        FaceDesc.pParent = this;
        FaceDesc.pParentTransform = m_pTransformCom;
        FaceDesc.vScale = { 1.f, 1.f, 1.f };
        FaceDesc.pBodyModelCom = m_pBodyModelCom;
        CStringHelper::ConvertUTFToWide(m_NpcDesc->szFaceName, FaceDesc.szFaceJsonDataName);
        if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Npc_Face"), TEXT("Part_Face"), &FaceDesc)))
            return E_FAIL;
    }

    if (strcmp("None", m_NpcDesc->szLeftWeaponPrototypeName))
    {
        CNpc_LeftWeaponPart::WEAPON_DESC LWeaponDesc = { };
        LWeaponDesc.pParent = this;
        LWeaponDesc.bIsApplyTransform = true;
        LWeaponDesc.pParentTransform = m_pTransformCom;
        LWeaponDesc.pSocketMatrix = m_pBodyModelCom->Get_BoneMatrixPtr(m_NpcDesc->szLeftBoneName);
        LWeaponDesc.vScale = { 1.f, 1.f, 1.f };
        if (1 == m_NpcDesc->iNpcID)
            LWeaponDesc.vRotation = {XMConvertToRadians(90.f), XMConvertToRadians(-90.f), XMConvertToRadians(0.f), 1.f};

        CStringHelper::ConvertUTFToWide(m_NpcDesc->szLeftWeaponPrototypeName, LWeaponDesc.szWeaponModelPrototype);
        LWeaponDesc.fSpeedPerSec = 5.f;
        if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Npc_Left_Weapon"), TEXT("Part_WeaponL"), &LWeaponDesc)))
            return E_FAIL;
    }

    if (strcmp("None", m_NpcDesc->szRightWeaponPrototypeName))
    {
        CNpc_RightWeaponPart::WEAPON_DESC RWeaponDesc = { };
        RWeaponDesc.pParent = this;
        RWeaponDesc.bIsApplyTransform = true;
        RWeaponDesc.pParentTransform = m_pTransformCom;
        RWeaponDesc.pSocketMatrix = m_pBodyModelCom->Get_BoneMatrixPtr(m_NpcDesc->szRightBoneName);
        RWeaponDesc.vScale = { 1.f, 1.f, 1.f };
        if(1 == m_NpcDesc->iNpcID)
            RWeaponDesc.vRotation = { XMConvertToRadians(90.f), XMConvertToRadians(-81.5f), XMConvertToRadians(0.f), 1.f };

        CStringHelper::ConvertUTFToWide(m_NpcDesc->szRightWeaponPrototypeName, RWeaponDesc.szWeaponModelPrototype);
        RWeaponDesc.fSpeedPerSec = 5.f;
        if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Npc_Right_Weapon"), TEXT("Part_WeaponR"), &RWeaponDesc)))
            return E_FAIL;
    }

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
   /* CInteraction_Component::INTERACTION_DESC InteractionDesc = {};
    InteractionDesc.vSize = Com_Size;
    InteractionDesc.BeginCallBackFunc = [&]() { this->Begin_Interaction(); };
    InteractionDesc.EndCallBackFunc = [&]() { this->End_Interaction(); };
    InteractionDesc.InteractionEvent = [&](_float fTimeDelta, CGameObject* pActionObject) { Excute_Interaction(fTimeDelta, pActionObject); };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Interaction"),
        TEXT("Com_Interaction"), reinterpret_cast<CComponent**>(&m_pInteractionCom), &InteractionDesc)))
        return E_FAIL;

    m_pInteractionCom->SetInteractionHitType(HIT_TYPE::INTERACTION);
    m_pInteractionCom->ADD_InteractionIgnoreObject(HIT_TYPE::NPC);*/

    /* Com_Interaction */
    CInteraction_Component::INTERACTION_DESC InteractionDesc = {};
    InteractionDesc.vSize = Com_Size;
    InteractionDesc.BeginCallBackFunc = [&]() { Begin_Interaction(); };
    InteractionDesc.EndCallBackFunc = [&]() { End_Interaction(); };
    InteractionDesc.InteractionEvent = [&](_float fTimeDelta, CGameObject* pActionObject) { Excute_Interaction(fTimeDelta, pActionObject); };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_InteractionBinder"),
        TEXT("Com_Interaction"), reinterpret_cast<CComponent**>(&m_pInteractionCom), &InteractionDesc)))
        return E_FAIL;
    m_pInteractionCom->Set_InterDesc(m_pGameManager->Find_InteractionData(m_NpcDesc->iInteractionID));
    m_pInteractionCom->SetOwner(this);

    m_pInteractionCom->SetInteractionHitType(HIT_TYPE::INTERACTION);
    m_pInteractionCom->ADD_InteractionIgnoreObject(HIT_TYPE::NPC);
    m_pInteractionCom->ADD_InteractionIgnoreObject(HIT_TYPE::MONSTER);

    if (5 == m_NpcDesc->iNpcID)
    {
        /* Drop Component */
        CDropComponent::DROP_COMPONENT_DESC DropComDesc = {};
        DropComDesc.fDropRange = 7.f;
        DropComDesc.fForce = 20.f;
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_DropComponent"),
            TEXT("Com_DropCom"), reinterpret_cast<CComponent**>(&m_pDropCom), &DropComDesc)))
            return E_FAIL;
    }

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
    if (m_pInteractionCom->Get_InterState() != INTERACTION_STATE::END)
        m_pGameInstance->ADD_Interaction(m_pInteractionCom);

    //// END일 땐 다시 안 보이게
    //if (m_pInteractionCom->Get_InterState() != INTERACTION_STATE::END)
    //    m_pInteractionCom->Set_InterState(INTERACTION_STATE::DEFAULT);
}

void CNpc::Excute_Interaction(_float fTimeDelta, CGameObject* pActionObject)
{
    // 여기서 플레이어 상태 처리 및 Lock 상태 관리
    if (!m_pInteractionCom->IsInteractionEnable())
        m_pInteractionCom->Set_Duration(m_pInteractionCom->Get_Duration() + fTimeDelta);

    if (INTERACTION_STATE::DEFAULT == m_pInteractionCom->Get_InterState())
    {
        if (m_pInteractionCom->IsInteractionEnable())
        {
            m_pInteractionCom->Set_InterState(INTERACTION_STATE::CONTACT);
        }
    }
    else if (INTERACTION_STATE::CONTACT == m_pInteractionCom->Get_InterState())
    {
        m_pInteractionCom->Set_InterState(INTERACTION_STATE::ACTIVE);
        m_pInteractionCom->Set_Duration(0.f);

        CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

        if (!pHUD)
        {
            Safe_Release(pHUD);
            return;
        }

        CUIScript* pScript = dynamic_cast<CUIScript*>(pHUD->Get_UIObject(TEXT("Layer_Script"), TEXT("UI_Scripts")));

        if (!pScript)
            return;

        pScript->Begin_Script(m_pGameManager->Get_ScriptData(m_NpcDesc->szScriptTags[0]));

        Safe_Release(pHUD);
    }
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

    Safe_Release(m_pDropCom);
    Safe_Release(m_pAIController);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pInteractionCom);
}
