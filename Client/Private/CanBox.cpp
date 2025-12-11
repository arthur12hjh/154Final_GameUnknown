#include "pch.h"
#include "CanBox.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "BoxOpenEvent.h"

#include "Interaction_Component.h"
#include "DropComponent.h"

#include "UIBase.h"
#include "UIHUD.h"

CCanBox::CCanBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CProb_Interaction(pDevice, pContext)
{
}

CCanBox::CCanBox(const CCanBox& Prototype) :
	CProb_Interaction(Prototype)
{
}

HRESULT CCanBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCanBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    PROB_INTERACTION_DESC* pDesc = static_cast<PROB_INTERACTION_DESC*>(pArg);
    pDesc->iInteractionID = 5;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(ADD_Components(*pDesc)))
        return E_FAIL;

    m_eInterState = INTERACTION_STATE::DEFAULT;
    m_pModelCom->Set_AnimationIndex(1, false);

    m_fInteractionDuration = _float2(0.f, 3.f);
    m_pCullingCollider->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
    return S_OK;
}

void CCanBox::Priority_Update(_float fTimeDelta)
{
}

void CCanBox::Update(_float fTimeDelta)
{
    if (m_pGameInstance->isIn_DistanceFrustum(m_pTransformCom->Get_State(STATE::POSITION), 150.f))
    {
        if (INTERACTION_STATE::ACTIVE == m_eInterState)
        {
            if (m_pModelCom->Play_Animation(fTimeDelta))
            {
                m_pDropCom->ItemDrop(3);
                m_eInterState = INTERACTION_STATE::END;
            }
        }
        else
        {
            auto pPlayerDesc = m_pGameManager->Get_PlayerDesc();
            if (PLAYER_MODE::IDLE == pPlayerDesc->ePlayerMode)
            {
                if (INTERACTION_STATE::LOCK == m_eInterState)
                {
                    // 나중에 여기서 조건 체크하세요
                    m_eInterState = INTERACTION_STATE::DEFAULT;
                }
            }
            else
            {
                if (INTERACTION_STATE::LOCK != m_eInterState)
                    m_eInterState = INTERACTION_STATE::LOCK;
            }
                
            m_pModelCom->Play_Animation(0.f);
        }
    }
}

void CCanBox::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	{
        if(INTERACTION_STATE::ACTIVE > m_eInterState)
		    m_pInteractionCom->Update_Com(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

#ifdef _DEBUG
		m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif

		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

HRESULT CCanBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
        if (FAILED(m_pModelCom->Bind_BoneSRV(i, m_pShaderCom, "g_BoneMatrixBuffer")))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_ORMTexture", aiTextureType_METALNESS, 0)))
            return E_FAIL;

        //if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, 0)))
        //    return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
            return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCanBox::ADD_Components(const PROB_INTERACTION_DESC& Desc)
{
    _float3 Com_Size = m_pTransformCom->Get_Scale();

    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), Desc.szVIBuffer_PrototypeName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Drop Component */
    CDropComponent::DROP_COMPONENT_DESC DropComDesc = {};
    DropComDesc.fDropRange = 7.f;
    DropComDesc.fForce = 20.f;
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_DropComponent"),
        TEXT("Com_DropCom"), reinterpret_cast<CComponent**>(&m_pDropCom), &DropComDesc)))
        return E_FAIL;

#pragma region DropItem Setting
    m_pDropCom->ADD_DropItem(make_pair(1, 30.f), 1);
    m_pDropCom->ADD_DropItem(make_pair(2, 30.f), 1);
    m_pDropCom->ADD_DropItem(make_pair(3, 10.f), 1);

#pragma endregion
    /* Com_Interaction */
    CInteraction_Component::INTERACTION_DESC InteractionDesc = {};
    InteractionDesc.vSize = Com_Size;
    InteractionDesc.BeginCallBackFunc = [&]() { this->Begin_OverlapCallBack(); };
    InteractionDesc.EndCallBackFunc = [&]() { this->End_OverlapCallBack(); };
    InteractionDesc.InteractionEvent = [&](_float fTimeDelta, CGameObject * pActionObject) { Excute_CallBack(fTimeDelta, pActionObject); };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Interaction"),
        TEXT("Com_Interaction"), reinterpret_cast<CComponent**>(&m_pInteractionCom), &InteractionDesc)))
        return E_FAIL;
    m_pInteractionCom->SetInteractionHitType(HIT_TYPE::INTERACTION);
    m_pInteractionCom->ADD_InteractionIgnoreObject(HIT_TYPE::MONSTER);

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

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

    m_pEventHandle = CBoxOpenEvent::Create([&](void* pArg) {});
    if (nullptr == m_pEventHandle)
        return E_FAIL;

    m_pGameInstance->Add_Event(TEXT("Box_Open"), m_pEventHandle);
    static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision({}, {}, Com_Size);
    return S_OK;
}

HRESULT CCanBox::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

void CCanBox::Excute_CallBack(_float fTimeDelta, CGameObject* pActionObject)
{
    // 여기서 플레이어 상태 처리 및 Lock 상태 관리
    if (m_fInteractionDuration.x <= m_fInteractionDuration.y)
        m_fInteractionDuration.x += fTimeDelta;

    if (INTERACTION_STATE::DEFAULT == m_eInterState)
    {
        if (IsInteractionEnable())
        {
            m_pModelCom->Set_AnimationIndex(1, false);

     /*     if (m_pEventHandle)
                m_pEventHandle->Notify(nullptr);*/
            m_eInterState = INTERACTION_STATE::CONTACT;
        }
    }
    else if (INTERACTION_STATE::CONTACT == m_eInterState)
    {
        m_eInterState = INTERACTION_STATE::ACTIVE;
        m_fInteractionDuration.x = 0.f;
        m_pGameInstance->Remove_Interaction(m_pInteractionCom);
    }
}

CCanBox* CCanBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCanBox* pCanBox = new CCanBox(pDevice, pContext);
    if (FAILED(pCanBox->Initialize_Prototype()))
    {
        Safe_Release(pCanBox);
        MSG_BOX("Create Fail : Can Box");
    }
    return pCanBox;
}

CGameObject* CCanBox::Clone(void* pArg)
{
    CCanBox* pCanBox = new CCanBox(*this);
    if (FAILED(pCanBox->Initialize(pArg)))
    {
        Safe_Release(pCanBox);
        MSG_BOX("Clone Fail : Can Box");
    }
    return pCanBox;
}

void CCanBox::Free()
{
    __super::Free();

    Safe_Release(m_pEventHandle);
    Safe_Release(m_pDropCom);
    Safe_Release(m_pModelCom);
}
