#include "pch.h"
#include "SciFi_Door.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "InteractionBinder.h"

#include "UIHUD.h"
#include "UIScript.h"
#include "UIActionEvent.h"

CSciFi_Door::CSciFi_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CProb_Interaction{ pDevice, pContext }
{
}

CSciFi_Door::CSciFi_Door(const CSciFi_Door& Prototype)
	: CProb_Interaction{ Prototype }
{
}

HRESULT CSciFi_Door::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSciFi_Door::Initialize(void* pArg)
{
	PROB_INTERACTION_DESC* pDesc = static_cast<PROB_INTERACTION_DESC*>(pArg);
	static_cast<PROB_INTERACTION_DESC*>(pArg)->iInteractionID = 4;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;

	if (FAILED(Ready_Col(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;

	m_pInteractionCom->Set_InterDesc(m_pGameManager->Find_InteractionData(pDesc->iInteractionID));
	m_pInteractionCom->Set_InterState(INTERACTION_STATE::DEFAULT);
	m_eCurState = SCIFI_DOOR_STATE::CLOSE;
	m_pModelCom->Set_AnimationIndex(0, false);
	m_pCullingCollider->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	m_pDoorEvent = CUIActionEvent::Create([&](void* pArg) { 
		UI_EVENT_ARG_DESC Desc = *static_cast<UI_EVENT_ARG_DESC*>(pArg);

		m_bCanlock = *static_cast<_bool*>(Desc.pData);
		});
	m_pGameInstance->Bind_Observer(TEXT("Get_Costume_Puzzle_Hint"), m_pDoorEvent);

	m_pUnlockEvent = CUIActionEvent::Create([&](void* pArg) {
		UI_EVENT_ARG_DESC Desc = *static_cast<UI_EVENT_ARG_DESC*>(pArg);

		m_bUnlocked = *static_cast<_bool*>(Desc.pData);
		});
	m_pGameInstance->Bind_Observer(TEXT("Get_Costume_Puzzle_Unlock"), m_pUnlockEvent);

	return S_OK;
}

void CSciFi_Door::Priority_Update(_float fTimeDelta)
{
}

void CSciFi_Door::Update(_float fTimeDelta)
{
	//m_pGameInstance->Manager_PlaySound(TEXT("MV_Xion01_PODFirstLanding_Main_door_1.wav"), CHANNELID::EFFECT);
	if (m_pGameInstance->isIn_DistanceFrustum(m_pTransformCom->Get_State(STATE::POSITION), 150.f))
	{
		if (INTERACTION_STATE::ACTIVE == m_pInteractionCom->Get_InterState() && m_bUnlocked && m_bCanlock)
		{
			if (m_eCurState == SCIFI_DOOR_STATE::CLOSE && !m_bIsSound)
			{
				m_pGameInstance->Manager_PlaySound(TEXT("MV_Xion01_PODFirstLanding_Main_door_2.wav"), CHANNELID::EFFECT, 20.f);
				m_bIsSound = true;
			}

			if (m_pModelCom->Play_Animation(fTimeDelta))
			{
				switch (m_eCurState)
				{
				case SCIFI_DOOR_STATE::OPEN:
				{
					m_pModelCom->Set_AnimationIndex(0, false);
					m_eCurState = SCIFI_DOOR_STATE::CLOSE;
					m_pRigidBody->Set_Simulation(true);
				}
				break;
				case SCIFI_DOOR_STATE::CLOSE:
				{
					m_pModelCom->Set_AnimationIndex(1, false);
					m_eCurState = SCIFI_DOOR_STATE::OPEN;
					m_pRigidBody->Set_Simulation(false);
				}
				break;
				}

				m_pInteractionCom->Set_InterState(INTERACTION_STATE::DEFAULT);
				m_bIsSound = false;
			}
		}
		else if (INTERACTION_STATE::ACTIVE == m_pInteractionCom->Get_InterState() && (!m_bUnlocked || !m_bCanlock))
		{
			CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

			if (!pHUD)
				return;

			if (!pHUD->Check_isOpenPopup(TEXT("UI_CostumePuzzlePopup"))
				&& pHUD->Get_UIObject(TEXT("Layer_Popup"), TEXT("UI_CostumePuzzlePopup"))->IsAnimFinished(TEXT("Popup_Close"))
				&& !dynamic_cast<CUIScript*>(pHUD->Get_UIObject(TEXT("Layer_Script"), TEXT("UI_Scripts")))->Get_Has_Script_Desc())
				m_pInteractionCom->Set_InterState(INTERACTION_STATE::DEFAULT);

			Safe_Release(pHUD);
		}
		else
		{
			auto pPlayerDesc = m_pGameManager->Get_PlayerDesc();
			if (PLAYER_MODE::BATTLE == pPlayerDesc->ePlayerMode || PLAYER_MODE::IDLE == pPlayerDesc->ePlayerMode)
			{
				if (INTERACTION_STATE::LOCK == m_pInteractionCom->Get_InterState())
				{
					// 나중에 여기서 조건 체크하세요
					m_pInteractionCom->Set_InterState(INTERACTION_STATE::DEFAULT);
				}
			}
			else
			{
				if (INTERACTION_STATE::LOCK != m_pInteractionCom->Get_InterState())
					m_pInteractionCom->Set_InterState(INTERACTION_STATE::LOCK);
			}

			m_pModelCom->Play_Animation(0.f);
		}
	}

}

void CSciFi_Door::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	{
		if (INTERACTION_STATE::ACTIVE > m_pInteractionCom->Get_InterState())
			m_pInteractionCom->Update_Com(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
#ifdef _DEBUG
		m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

HRESULT CSciFi_Door::Render()
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

HRESULT CSciFi_Door::Ready_Components(const _tchar* pComponentTag)
{
	auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
	pCullingCollider->SetCollision({ 0.f, 5.f, 0.f }, {}, { 10.f, 10.f, 2.f });

	_float3 Com_Size = pCullingCollider->GetOrizinBounding().Extents;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Interaction */
	CInteraction_Component::INTERACTION_DESC InteractionDesc = {};
	InteractionDesc.vSize = Com_Size;
	InteractionDesc.BeginCallBackFunc = [&]() { this->Begin_OverlapCallBack(); };
	InteractionDesc.EndCallBackFunc = [&]() { this->End_OverlapCallBack(); };
	InteractionDesc.InteractionEvent = [&](_float fTimeDelta, CGameObject* pActionObject) { Excute_CallBack(fTimeDelta, pActionObject); };

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_InteractionBinder"),
		TEXT("Com_Interaction"), reinterpret_cast<CComponent**>(&m_pInteractionCom), &InteractionDesc)))
		return E_FAIL;
	
	m_pInteractionCom->SetInteractionHitType(HIT_TYPE::INTERACTION);
	m_pInteractionCom->ADD_InteractionIgnoreObject(HIT_TYPE::MONSTER);

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSciFi_Door::Ready_Col(const _tchar* pComponentTag)
{
	/* Com_Model_COL */
	_wstring strComponentTag = pComponentTag;
	strComponentTag += TEXT("_COL");


	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), strComponentTag,
		TEXT("Com_Model_COL"), reinterpret_cast<CComponent**>(&m_pColModelCom))))
		return E_FAIL;

	PxUserData tUserData;
	// 엘레베이터 식별용 문자열. 이건 나중에 엘베말고 다른데에 넣을떄 저랑 얘기하고 정해서 넣어주세요
	tUserData.szActorTag = TEXT("Door_Actor");

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

HRESULT CSciFi_Door::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

void CSciFi_Door::Excute_CallBack(_float fTimeDelta, CGameObject* pActionObject)
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

		if (!m_bUnlocked && m_bCanlock) // 비밀번호 입력 가능 상태일 때
		{
			// 퍼즐 팝업 열기
			pHUD->Open_Popup(TEXT("UI_CostumePuzzlePopup"));
		}
		else if (!m_bUnlocked && !m_bCanlock) // 절대 열 수 없을 때
		{
			CUIScript* pScript = dynamic_cast<CUIScript*>(pHUD->Get_UIObject(TEXT("Layer_Script"), TEXT("UI_Scripts")));

			if (!pScript)
				return;

			pScript->Begin_Script(m_pGameManager->Get_ScriptData(TEXT("DoorInteractionScript")));
		}
		Safe_Release(pHUD);
	}
}

CSciFi_Door* CSciFi_Door::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSciFi_Door* pInstance = new CSciFi_Door(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSciFi_Door::Clone(void* pArg)
{
	CSciFi_Door* pInstance = new CSciFi_Door(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSciFi_Door");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSciFi_Door::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pDoorEvent);
	Safe_Release(m_pUnlockEvent);
}
