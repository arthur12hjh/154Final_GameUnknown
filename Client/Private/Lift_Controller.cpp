#include "pch.h"
#include "Lift_Controller.h"

#include "GameInstance.h"
#include "Interaction_Component.h"
#include "Lift_Platform.h"

#include "UIBase.h"
#include "UIHUD.h"
#include "UISimpleKey.h"

#include "GameManager.h"

CLift_Controller::CLift_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CProb_Interaction{ pDevice, pContext }
{
}

CLift_Controller::CLift_Controller(const CLift_Controller& Prototype)
	: CProb_Interaction{ Prototype }
{
}

HRESULT CLift_Controller::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLift_Controller::Initialize(void* pArg)
{
	PROB_INTERACTION_DESC* pDesc = static_cast<PROB_INTERACTION_DESC*>(pArg);
	static_cast<PROB_INTERACTION_DESC*>(pArg)->iInteractionID = 1;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;

	ResetAction(true);
	m_bIsControllLift = true;
	m_eControllState = LIFT_CONTROLL_STATE::LIFT_UP;

	return S_OK;
}

void CLift_Controller::Priority_Update(_float fTimeDelta)
{
}

void CLift_Controller::Update(_float fTimeDelta)
{
	_matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());

	if (m_bIsControllLift)
	{
		if (m_pLiftPlatform)
		{
			_matrix vPlatformMatrix = XMLoadFloat4x4(m_pLiftPlatform->GetTransform()->Get_WorldMatrixPtr());
			for (_uint i = 0; i < 3; ++i)
				vPlatformMatrix.r[i] = XMVector3Normalize(vPlatformMatrix.r[i]);

			WorldMat = WorldMat * vPlatformMatrix;
			XMStoreFloat4x4(&m_CombinedMatrix, WorldMat);
		}
	}
	
	m_pCullingCollider->UpdateColiision(WorldMat);
	m_pRigidBody->Update_PxTransform(WorldMat);


	if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_0))
	{
		Excute_CallBack(nullptr);
	}
	//m_pInteractionCom->Update_Com();
	m_pModelCom->Play_Animation(fTimeDelta);
	ResetAction();

	if (m_pLiftPlatform && m_eInterState == INTERACTION_STATE::ACTIVE)
	{
		if (m_pLiftPlatform->GetPlatformMove())
			m_eInterState = INTERACTION_STATE::END;
		else
			m_eInterState = INTERACTION_STATE::DEFAULT;
	}
}

void CLift_Controller::Late_Update(_float fTimeDelta)
{
	/*if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
		m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
	}*/
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

}

HRESULT CLift_Controller::Render()
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

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CLift_Controller::SetControllPlatform(CLift_Platform* pControllPlatform)
{
	//XMStoreFloat4x4(&m_PlatformLocalMat, XMMatrixIdentity());
	//memcpy(&m_PlatformLocalMat, m_pTransformCom->Get_WorldMatrixPtr(), sizeof(_float4x4));

	m_pTransformCom->Set_State(STATE::POSITION, { 7.5f, 0.f, 0.f, 1.f });
	//m_PlatformLocalMat._41 = 7.5f;
	//m_PlatformLocalMat._42 = 0.f;
	//m_PlatformLocalMat._43 = 0.f;
}

HRESULT CLift_Controller::Ready_Components(const _tchar* pComponentTag)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Interaction */
	CInteraction_Component::INTERACTION_DESC InteractionDesc = {};
	InteractionDesc.vSize = {2.f, 2.f, 2.f};
	InteractionDesc.BeginCallBackFunc = [&]() { this->Begin_OverlapCallBack(); };
	InteractionDesc.EndCallBackFunc = [&]() { this->End_OverlapCallBack(); };
	InteractionDesc.InteractionEvent = [&](CGameObject* pActionObject) { this->Excute_CallBack(pActionObject); };

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Interaction"),
		TEXT("Com_Interaction"), reinterpret_cast<CComponent**>(&m_pInteractionCom), &InteractionDesc)))
		return E_FAIL;
	m_pInteractionCom->SetInteractionHitType(HIT_TYPE::INTERACTION);
	m_pInteractionCom->ADD_InteractionOnlyHitObject(HIT_TYPE::PLAYER);

	static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision({}, {}, { 2.f, 2.f, 2.f });

	auto pPlatformList = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Lift_Platform"));
	if (pPlatformList)
	{
		if (false == pPlatformList->empty())
			SetControllPlatform(static_cast<CLift_Platform*>(pPlatformList->front()));
	}

	/* Com_Model_COL */
	_wstring strComponentTag = pComponentTag;
	strComponentTag += TEXT("_COL");

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), strComponentTag,
		TEXT("Com_Model_COL"), reinterpret_cast<CComponent**>(&m_pColModelCom))))
		return E_FAIL;

	// 충돌용 메시 피직스 세팅 조건
	// 1. 충돌용 메시 생성 이후에 RigidBody를 세팅해주셔야합니다.
	// 2. 해당 객체의 위치가 설정된 뒤에 RigidBody를 세팅해주셔야 합니다.

	// 세팅 방법 보고도 잘 이해 안되면 물어봐주세요 키네마틱, 다이나믹, 스태틱 세팅 중요해요
	PxUserData tUserData;
	// 엘레베이터 식별용 문자열. 이건 나중에 엘베말고 다른데에 넣을떄 저랑 얘기하고 정해서 넣어주세요
	tUserData.szActorTag = TEXT("Elevator_Actor");

	// 리지드 바디 Desc 세팅. 
	// F12 타고 들어가서 머테리얼이랑 Mass, userdata, shape, type 부분 위주로 살펴보세요.
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc;
	// 콜라이더 모양.
	// ->> 메시는 TRIANGLE, 박스나 캡슐은 BOX, CAPSULE 다 따로 있으니까
	// F12 타고 들어가서 한번 확인해보세요.
	RigidBodyDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::TRIANGLE;

	// 충돌처리를 할지말지 
	// DYNAMIC : 충돌 
	// KINEMATIC : 충돌 X
	// STATIC : 충돌 O, 대신 고정되어 있음.
	// ->> 엘레베이터는 고정되어있으니까 STATIC으로 세팅 해주는거에요.

	RigidBodyDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::STATIC;

	RigidBodyDesc.StartWorldMatrix = *m_pTransformCom->Get_WorldMatrixPtr();
	RigidBodyDesc.tUserData = tUserData;
	RigidBodyDesc.vMaterial = _float3(0.5f, 0.5f, 0.3f);
	RigidBodyDesc.vSize = m_pTransformCom->Get_Scale();
	RigidBodyDesc.fMass = { 0.3f };
	// TRIANGLE로 세팅하고 충돌용 메시 작업하는거니까, 충돌용 메시 넣어줘야돼요 
	// TRIANGLE 타입이 아닌 (충돌용 메시가 아닌) 녀석들은 굳이 안넣어줘도 됩니다.
	RigidBodyDesc.pColModel = m_pColModelCom;

	/* Com_RigidBody */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
		return E_FAIL;

	// 리지드 바디 세팅 끝났으면 Physx 매니저에 집어넣는 과정도 있어야돼요.
	// 없으면 충돌 안됨
	m_pGameInstance->Add_RigidBody_ToPhysx(this, m_pRigidBody);

	return S_OK;
}

HRESULT CLift_Controller::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (m_bIsControllLift)
	{
		if (m_pLiftPlatform)
		{
			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedMatrix)))
				return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLift_Controller::Begin_OverlapCallBack()
{
	m_pGameInstance->ADD_Interaction(m_pInteractionCom);

	m_eInterState = INTERACTION_STATE::DEFAULT;


	return S_OK;
}

HRESULT CLift_Controller::End_OverlapCallBack()
{
	m_pGameInstance->Remove_Interaction(m_pInteractionCom);

	m_eInterState = INTERACTION_STATE::END;


	return S_OK;
}

void CLift_Controller::Excute_CallBack(CGameObject* pActionObject)
{
	if (LIFT_ANIM_STATE::LIFT_ANIM_PUSH == m_eCurState)
	{
		m_eInterState = INTERACTION_STATE::ACTIVE;

		if (m_pLiftPlatform)
		{
			if (m_pLiftPlatform->SetPlatformMove(CLift_Platform::LIFT_PLATFORM_STATE(ENUM_CLASS(m_eControllState))))
			{
				m_pModelCom->Set_AnimationIndex(1, false);
				m_eCurState = LIFT_ANIM_STATE::LIFT_ANIM_PULL;
				m_pModelCom->Set_AnimationIndex(ENUM_CLASS(m_eCurState), false);
			}

			if (m_bIsControllLift)
			{
				if (LIFT_CONTROLL_STATE::LIFT_UP == m_eControllState)
					m_eControllState = LIFT_CONTROLL_STATE::LIFT_DOWN;
				else if (LIFT_CONTROLL_STATE::LIFT_DOWN == m_eControllState)
					m_eControllState = LIFT_CONTROLL_STATE::LIFT_UP;
			}
		}
		else
		{
			auto pPlatformList = m_pGameInstance->GetAllObejctToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Lift_Platform"));
			if (pPlatformList)
			{
				if (false == pPlatformList->empty())
					m_pLiftPlatform = static_cast<CLift_Platform*>(pPlatformList->front());
			}
		}
	}
}

void CLift_Controller::ResetAction(_bool bIsForce)
{
	_bool bIsAction = false;
	if (LIFT_ANIM_STATE::LIFT_ANIM_PULL == m_eCurState)
	{
		if (m_pModelCom->IsAnimationFinished())
			bIsAction = true;
	}

	if (bIsAction || bIsForce)
	{
		m_eCurState = LIFT_ANIM_STATE::LIFT_ANIM_PUSH;
		m_pModelCom->Set_AnimationIndex(ENUM_CLASS(m_eCurState), false, 1.f, 0.12f, false, 34.f, 34.f);
	}
}

CLift_Controller* CLift_Controller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLift_Controller* pInstance = new CLift_Controller(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLift_Controller::Clone(void* pArg)
{
	CLift_Controller* pInstance = new CLift_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLift_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLift_Controller::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pLiftPlatform);
}
