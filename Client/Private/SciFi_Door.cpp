#include "pch.h"
#include "SciFi_Door.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Interaction_Component.h"

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

	m_eInterState = INTERACTION_STATE::DEFAULT;
	m_eCurState = SCIFI_DOOR_STATE::CLOSE;
	m_pModelCom->Set_AnimationIndex(0, false);
	m_pCullingCollider->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	return S_OK;
}

void CSciFi_Door::Priority_Update(_float fTimeDelta)
{
}

void CSciFi_Door::Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_DistanceFrustum(m_pTransformCom->Get_State(STATE::POSITION), 150.f))
	{
		if (INTERACTION_STATE::ACTIVE == m_eInterState)
		{
			if (m_pModelCom->Play_Animation(fTimeDelta))
				m_eInterState = INTERACTION_STATE::DEFAULT;
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

void CSciFi_Door::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	{
		if (INTERACTION_STATE::ACTIVE > m_eInterState)
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
	_float3 Com_Size = m_pTransformCom->Get_Scale();

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Interaction */
	CInteraction_Component::INTERACTION_DESC InteractionDesc = {};
	InteractionDesc.vSize = Com_Size;
	InteractionDesc.BeginCallBackFunc = [&]() { this->Begin_OverlapCallBack(); };
	InteractionDesc.EndCallBackFunc = [&]() { this->End_OverlapCallBack(); };
	InteractionDesc.InteractionEvent = [&](_float fTimeDelta, CGameObject* pActionObject) { Excute_CallBack(fTimeDelta, pActionObject); };

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
	static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision({}, {}, Com_Size);
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
	if (m_fInteractionDuration.x <= m_fInteractionDuration.y)
		m_fInteractionDuration.x += fTimeDelta;

	if (INTERACTION_STATE::DEFAULT == m_eInterState)
	{
		if (IsInteractionEnable())
		{
			
			m_eInterState = INTERACTION_STATE::CONTACT;
		}
	}

	if (INTERACTION_STATE::CONTACT == m_eInterState)
	{
		switch (m_eCurState)
		{
		case SCIFI_DOOR_STATE::OPEN:
		{
			m_pModelCom->Set_AnimationIndex(1, false);
			m_eCurState = SCIFI_DOOR_STATE::CLOSE;
		}
			break;
		case SCIFI_DOOR_STATE::CLOSE:
		{
			m_pModelCom->Set_AnimationIndex(0, false);
			m_eCurState = SCIFI_DOOR_STATE::OPEN;
		}
			break;
		}
			
		m_eInterState = INTERACTION_STATE::ACTIVE;
		m_fInteractionDuration.x = 0.f;
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
}
