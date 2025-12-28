#include "pch.h"
#include "StaticInteraction.h"

#include "GameInstance.h"
#include "UIBase.h"
#include "GameManager.h"
#include "InteractionUIBinder.h"
#include "UIScript.h"
#include "UIHUD.h"

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

    PROB_INTERACTION_DESC* pDesc = static_cast<PROB_INTERACTION_DESC*>(pArg);

    SetCullingCollider(pDesc->iObjectID);

    if (FAILED(ADD_Components(*pDesc)))    
        return E_FAIL;

    if (nullptr == wcsstr(pDesc->szVIBuffer_PrototypeName, TEXT("Corpse")))
    {
        if (FAILED(Ready_COL(*pDesc)))
            return E_FAIL;
    }

    _matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    m_pCullingCollider->UpdateColiision(WorldMat);

    if (m_pRigidBody)
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
        if (m_pInteractionCom->Get_InterState() == INTERACTION_STATE::ACTIVE)
        {
            if (m_pInteractionCom->Get_InterDesc()->eType == INTERACTION_TYPE::CORPSE) // 시체 상호작용 처리
            {
                CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

                if (!pHUD)
                {
                    Safe_Release(pHUD);
                    return;
                }

                if (!pHUD->Check_isOpenPopup(TEXT("UI_CostumePuzzleHintPopup"))
                    && pHUD->Get_UIObject(TEXT("Layer_Popup"), TEXT("UI_CostumePuzzleHintPopup"))->IsAnimFinished(TEXT("Popup_Close")))
                    m_pInteractionCom->Set_InterState(INTERACTION_STATE::DEFAULT);
            
                Safe_Release(pHUD);
            }
            
            if (m_pInteractionCom->Get_InterDesc()->eType == INTERACTION_TYPE::VENDING_MACINE) // 자판기 상호작용 처리
            {
                CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

                if (!pHUD)
                {
                    Safe_Release(pHUD);
                    return;
                }

                if (!pHUD->Check_isOpenPopup(TEXT("UI_Map_Selector_Popup"))
                    && pHUD->Get_UIObject(TEXT("Layer_Popup"), TEXT("UI_Map_Selector_Popup"))->IsAnimFinished(TEXT("Popup_Close")))
                    m_pInteractionCom->Set_InterState(INTERACTION_STATE::DEFAULT);
            
                Safe_Release(pHUD);
            }
        }
        m_pInteractionCom->Update_Com(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
      
#ifdef _DEBUG
        m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
        m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
        m_pGameInstance->Add_RenderGroup(RENDER::OCCLUSION, this);
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
        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORMTexture", aiTextureType_METALNESS, 0)))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CStaticInteraction::ADD_Components(const PROB_INTERACTION_DESC& Desc)
{
    _float3 Com_Size = m_pTransformCom->Get_Scale();
    Com_Size.x *= 2.f;
    Com_Size.z *= 2.f;

    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), Desc.szVIBuffer_PrototypeName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Interaction */
    CInteraction_Component::INTERACTION_DESC InteractionDesc = {};
    InteractionDesc.vSize = Com_Size;
    InteractionDesc.BeginCallBackFunc = [&]() { Begin_OverlapCallBack(); };
    InteractionDesc.EndCallBackFunc = [&]() { End_OverlapCallBack(); };
    InteractionDesc.InteractionEvent = [&](_float fTimeDelta, CGameObject* pActionObject) { Excute_CallBack(fTimeDelta, pActionObject); };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_InteractionUIBinder"),
        TEXT("Com_Interaction"), reinterpret_cast<CComponent**>(&m_pInteractionCom), &InteractionDesc)))
        return E_FAIL;
    m_pInteractionCom->Set_InterDesc(m_pGameManager->Find_InteractionData(Desc.iInteractionID));
    m_pInteractionCom->SetOwner(this);
    m_pInteractionCom->ADD_InteractionIgnoreObject(HIT_TYPE::MONSTER);

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
  

    return S_OK;
}

HRESULT CStaticInteraction::Ready_COL(const PROB_INTERACTION_DESC& Desc)
{
    _float3 Com_Size = m_pTransformCom->Get_Scale();
    Com_Size.x *= 2.f;
    Com_Size.z *= 2.f;

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

void CStaticInteraction::SetCullingCollider(_uint iObjectID)
{
    auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
    switch (iObjectID)
    {
    case 1: // VendingMachine_6A (1, 2, 1)
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.5f }, {}, { 1.f, 2.f, 1.f });
        break;
    case 2: // VendingMachine_7A (2, 3, 2)
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 2.f, 3.f, 2.f });
        break;
    case 3: // Camp_1I (1, 1, 1)
        pCullingCollider->SetCollision({ 0.f, 1.f, 0.f }, {}, { 1.f, 1.f, 1.f });
        break;
    case 4: // Corpse_1A (1, 1, 1.5)
        pCullingCollider->SetCollision({ 0.f, 1.f, -0.5f }, {}, { 1.f, 2.f, 1.5f });
        break;
    case 5: // Corpse_1B (1, 1, 2.2)
        pCullingCollider->SetCollision({ -0.2f, 0.f, 0.1f }, {}, { 1.f, 1.f, 2.2f });
        break;
    case 6: // Corpse_2A (2.2, 1, 1.5)
    case 7: // Corpse_2B (2.2, 1, 1.5)
        pCullingCollider->SetCollision({ 0.f, 0.f, 0.f }, {}, { 2.2f, 1.f, 1.5f });
        break;
    case 8: // Corpse_2C (1.5, 1.5, 1.5)
        pCullingCollider->SetCollision({ 0.f, 0.5f, 0.f }, {}, { 1.5f, 1.5f, 1.5f });
        break;
    case 9: // Corpse_3B (4, 3, 4)
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.5f }, {}, { 4.f, 3.f, 4.f });
        break;
    }
}

HRESULT CStaticInteraction::Begin_OverlapCallBack()
{
    __super::Begin_OverlapCallBack();

    return S_OK;
}

void CStaticInteraction::Excute_CallBack(_float fTimeDelta, CGameObject* pActionObject)
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

        if (m_pInteractionCom->Get_InterDesc()->eType == INTERACTION_TYPE::CORPSE) // 시체 상호작용 처리
        {
            CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

            if (!pHUD)
            {
                Safe_Release(pHUD);
                return;
            }

            if (m_bHasHint)
            {
                pHUD->Open_Popup(TEXT("UI_CostumePuzzleHintPopup"));
            }
            else
            {
                /* CUIScript* pScript = dynamic_cast<CUIScript*>(pHUD->Get_UIObject(TEXT("Layer_Script"), TEXT("UI_Scripts")));

                if (!pScript)
                    return;

                pScript->Begin_Script(m_pGameManager->Get_ScriptData(TEXT("CorpseInteractionScript")));*/
            }
            Safe_Release(pHUD);
        }
        else if (m_pInteractionCom->Get_InterDesc()->eType == INTERACTION_TYPE::VENDING_MACINE) // 자판기 상호작용 처리
        {
            CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

            if (!pHUD)
            {
                Safe_Release(pHUD);
                return;
            }

            pHUD->Open_Popup(TEXT("UI_Map_Selector_Popup"));

            Safe_Release(pHUD);
        }
    }    
}

HRESULT CStaticInteraction::End_OverlapCallBack()
{
    __super::End_OverlapCallBack();
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
