#include "pch.h"
#include "Npc.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "StringHelper.h"
#include "InteractionBinder.h"

#include "Player.h"
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

#include "Camera_Npc.h"
#include "NpcEvent.h"
#include "UIShop.h"

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

    if (m_pGameManager->GetLevelTransportIndex() == 2 && m_NpcDesc->iNpcID == ENUM_CLASS(NPC_ID::DORORONG) && !m_isReturnDororong)
    {
        CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

        if (!pHUD)
            return;

        if (pHUD->Check_AnimFinish(TEXT("Layer_Combat"), TEXT("GamePlay_Overlay"), TEXT("Intro")))
        {
            m_isReturnDororong = true;

            m_pInteractionCom->Set_InterState(INTERACTION_STATE::CONTACT);
            auto pPlayer = m_pGameManager->GetGameCharacter();
            
            if (!pPlayer)
                return;

            m_pInteractionCom->Action_InteractionEvent(fTimeDelta, pPlayer);

            Safe_Release(pPlayer);
        }
        Safe_Release(pHUD);
    }

    m_pCCT->Update_PrePxPosition(m_pTransformCom);
    m_pAIController->Priority_Update(fTimeDelta);

}

void CNpc::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if (m_pGameInstance->isIn_DistanceFrustum(m_pTransformCom->Get_State(STATE::POSITION), 150.f))
    {
        if (INTERACTION_STATE::CONTACT == m_pInteractionCom->Get_InterState())
        {
            m_pInteractionCom->Set_InterState(INTERACTION_STATE::ACTIVE);
            m_pInteractionCom->Set_Duration(0.f);

            Change_Camera();
            m_eNpcState = NPC_STATE::MEAT;
        }
        else if (INTERACTION_STATE::ACTIVE == m_pInteractionCom->Get_InterState())
        {
            Npc_Action();
        }
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

        m_pNpcEvent = CNpcEvent::Create(nullptr);
        m_pGameInstance->Add_Event(TEXT("Go_DororongSaber"), m_pNpcEvent);
    }

    /* Com_CCT */
    CCharacterController::CCT_DESC Desc;
    PxUserData tUserData;
    tUserData.szActorTag = TEXT("Player_CCT");

    Desc.eCharacterControllerType = CCharacterController::CCT_SHAPE::CAPSULE;
    Desc.tUserData = tUserData;
    //ĸ�� ��Ʈ�ѷ����� x�� �� ���� y�� ��� ����
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

    //// END�� �� �ٽ� �� ���̰�
    //if (m_pInteractionCom->Get_InterState() != INTERACTION_STATE::END)
    //    m_pInteractionCom->Set_InterState(INTERACTION_STATE::DEFAULT);
}

void CNpc::Excute_Interaction(_float fTimeDelta, CGameObject* pActionObject)
{
    // ���⼭ �÷��̾� ���� ó�� �� Lock ���� ����
    if (!m_pInteractionCom->IsInteractionEnable())
        m_pInteractionCom->Set_Duration(m_pInteractionCom->Get_Duration() + fTimeDelta);

    if (INTERACTION_STATE::DEFAULT == m_pInteractionCom->Get_InterState())
    {
        if (m_pInteractionCom->IsInteractionEnable())
        {
            m_pInteractionCom->Set_InterState(INTERACTION_STATE::CONTACT);
        }
    }
    /*else if (INTERACTION_STATE::CONTACT == m_pInteractionCom->Get_InterState())
    {
        m_pInteractionCom->Set_InterState(INTERACTION_STATE::ACTIVE);
        m_pInteractionCom->Set_Duration(0.f);
        
        Change_Camera();
        m_eNpcState = NPC_STATE::MEAT;
    }*/
}

void CNpc::End_Interaction()
{
    m_pGameInstance->Remove_Interaction(m_pInteractionCom);
}

void CNpc::Npc_Action()
{
    CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

    if (!pHUD)
        return;

    CUIScript* pScript = dynamic_cast<CUIScript*>(pHUD->Get_UIObject(TEXT("Layer_Script"), TEXT("UI_Scripts")));

    if (!pScript)
        return;

    auto pNpcCamera = dynamic_cast<CCamera_Npc*>(m_pGameInstance->GetMainCamera());

    if (!pNpcCamera)
        return;

    switch (m_eNpcState)
    {
    case NPC_STATE::IDLE:
        break;
    case NPC_STATE::MEAT:
    {
        if (pNpcCamera->Get_LerpEnd())
        {
            pScript->Begin_Script(m_pGameManager->Get_ScriptData(m_NpcDesc->szScriptTags[m_iScriptIdx]));
            m_eNpcState = NPC_STATE::TALK;
        }
        break;
    }
    case NPC_STATE::TALK:
    {
        if (!pScript->Get_Has_Script_Desc())
        {
            if (m_NpcDesc->iNpcID == ENUM_CLASS(NPC_ID::SCARLET))
                m_eNpcState = NPC_STATE::BYE;

            if (m_NpcDesc->iNpcID == ENUM_CLASS(NPC_ID::DIG))
            {
                if (m_iScriptIdx == 0)
                    m_eNpcState = NPC_STATE::ACTION_BEGIN;
                else if(m_iScriptIdx > 0)
                    m_eNpcState = NPC_STATE::BYE;
            }

            if (m_NpcDesc->iNpcID == ENUM_CLASS(NPC_ID::DORORONG)) // ���� npc || // ���η� npc
            {
                if (m_iScriptIdx == 0)
                    m_eNpcState = NPC_STATE::ACTION_BEGIN;
                else if (m_iScriptIdx > 0)
                    m_eNpcState = NPC_STATE::BYE;
            }
        }
        break;
    }
    case NPC_STATE::ACTION_BEGIN:
    {
        if (m_NpcDesc->iNpcID == ENUM_CLASS(NPC_ID::DIG)) // ���� npc
        {
            pHUD->Open_Shop();
            m_eNpcState = NPC_STATE::ACTION;
        }

        if (m_NpcDesc->iNpcID == ENUM_CLASS(NPC_ID::DORORONG) && m_iScriptIdx == 0) // ���η� npc
        {
            LEVEL_CHANGER LevelChanger{ true, ENUM_CLASS(LEVEL::BEATSABER_GAME) };

            UI_EVENT_ARG_DESC Arg{};
            Arg.Type = UI_EVENT_ARG_DESC::LEVEL_CHANGER;
            Arg.pData = &LevelChanger;
            m_pNpcEvent->Notify(&Arg);
        }
        break;
    }
    case NPC_STATE::ACTION:
    {
        if (m_NpcDesc->iNpcID == ENUM_CLASS(NPC_ID::DIG)) // ���� npc
        {
            if(!dynamic_cast<CUIShop*>(pHUD->Get_UIObject(TEXT("Layer_Shop"), TEXT("UI_Shop")))->Get_IsOpen())
                m_eNpcState = NPC_STATE::ACTION_END;
        }
        break;
    }
    case NPC_STATE::ACTION_END:
    {
        if (m_NpcDesc->iNpcID == ENUM_CLASS(NPC_ID::DIG))
        {
            if (m_iScriptIdx == 0)
            {
                ++m_iScriptIdx;
                pScript->End_Script();
                pScript->Begin_Script(m_pGameManager->Get_ScriptData(m_NpcDesc->szScriptTags[m_iScriptIdx]));
                m_eNpcState = NPC_STATE::TALK;
            }
            else
                m_eNpcState = NPC_STATE::BYE;
        }
        break;
    }
    case NPC_STATE::BYE:
    {
        if (m_NpcDesc->iNpcID == ENUM_CLASS(NPC_ID::DORORONG)) // ���η� npc
        {
            m_pDropCom->DropRewardItem();
        }

        Return_Camera();
        m_eNpcState = NPC_STATE::IDLE;
        m_pInteractionCom->Set_InterState(INTERACTION_STATE::DEFAULT);
        break;
    }
    }

    Safe_Release(pNpcCamera);
    Safe_Release(pHUD);
}

void CNpc::Change_Camera()
{
    _float4x4 matPlayerCamera{};
    XMStoreFloat4x4(&matPlayerCamera, m_pGameInstance->GetCameraWorldMatrix(TEXT("PlayerCamera")));

    _float4 vPos{};
    XMStoreFloat4(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    vPos.x += m_NpcDesc->vCamTargetViewPosOffset.x;
    vPos.y += m_NpcDesc->vCamTargetViewPosOffset.y;
    vPos.z += m_NpcDesc->vCamTargetViewPosOffset.z;

    auto pPlayerCamera = m_pGameInstance->GetMainCamera();

    if (!pPlayerCamera)
        return;

    _vector vPrevLook = pPlayerCamera->GetTransform()->Get_State(STATE::LOOK);
    auto pPlayer = m_pGameManager->GetGameCharacter();

    if (!pPlayer)
        return;

    pPlayer->SetActive(false);

    m_pGameInstance->SetMainCamera(TEXT("NpcCamera"));
    auto pNpcCamera = dynamic_cast<CCamera_Npc*>(m_pGameInstance->GetMainCamera());

    if (!pNpcCamera)
        return;

    pNpcCamera->SetTargetNpc(this);
    pNpcCamera->SetPrevLook(vPrevLook);

    _float4x4 matNpcCam{};
    XMStoreFloat4x4(&matNpcCam, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    _vector vTargetPos = m_pTransformCom->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::LOOK) * m_NpcDesc->fCamTargetDist;

    _float4 vResult{};
    XMStoreFloat4(&vResult, vTargetPos);

    matNpcCam.m[3][0] = vResult.x + m_NpcDesc->vCamTargetPosOffset.x;
    matNpcCam.m[3][1] = vResult.y + m_NpcDesc->vCamTargetPosOffset.y;
    matNpcCam.m[3][2] = vResult.z + m_NpcDesc->vCamTargetPosOffset.z;

    pNpcCamera->SetCameraAnimation(&matPlayerCamera, &matNpcCam, vPos, (m_NpcDesc->fCamTargetDist * 0.5f));

    m_iScriptIdx = 0;

    if (m_pGameManager->GetLevelTransportIndex() == 2 && m_NpcDesc->iNpcID == ENUM_CLASS(NPC_ID::DORORONG))
        m_iScriptIdx = 1;

    Safe_Release(pPlayer);
    Safe_Release(pPlayerCamera);
    Safe_Release(pNpcCamera);
}

void CNpc::Return_Camera()
{
    auto pNpcCamera = dynamic_cast<CCamera_Npc*>(m_pGameInstance->GetMainCamera());
    if (!pNpcCamera)
        return;

    auto pPlayer = CGameManager::GetInstance()->GetGameCharacter();

    if (!pPlayer)
        return;

    pPlayer->SetActive(true);
    Safe_Release(pPlayer);
    
    pNpcCamera->ReverseCameraAnimation();
    Safe_Release(pNpcCamera);
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

    if(m_pDropCom)
        Safe_Release(m_pDropCom);

    Safe_Release(m_pAIController);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pInteractionCom);
    Safe_Release(m_pNpcEvent);
}
