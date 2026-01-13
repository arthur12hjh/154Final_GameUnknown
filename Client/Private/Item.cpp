#include "pch.h"
#include "Item.h"

#include "GameInstance.h"
#include "InteractionBinder.h"
#include "Effect.h"
#include "UIHUD.h"
#include "UIGetterQueue.h"
#include "GameManager.h"
#include "Player.h"
#include "TrailEffect.h"

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CProb_Interaction(pDevice, pContext)
{
}

CItem::CItem(const CItem& Prototype) :
	CProb_Interaction(Prototype)
{
}

HRESULT CItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ITEM_DESC* pDesc = static_cast<ITEM_DESC*>(pArg);
	m_bIsLerpAnimation = true;
	m_fAmount = pDesc->fAmount;
	m_fDropForce = pDesc->fDropForce;
	m_isHemiSphere = pDesc->isHemiSphere;
	m_vParentLook = pDesc->vParentLook;
	//_vector vOwnerPos = m_pTransformCom->Get_State(STATE::POSITION);
	//_vector vDropPoint = XMLoadFloat3(&pDesc->fDropPoint);

	//m_pTransformCom->Set_State(STATE::POSITION, vOwnerPos + XMVectorSet(0.f, 2.f, 0.f, 0.f));

	//_vector vBezirPoint = XMVectorLerp(vOwnerPos, vDropPoint, 1.f);
	//vBezirPoint.m128_f32[1] += 5.f;
	//XMStoreFloat3(&m_CurvePoins[4], vDropPoint);

	//_vector vCenterPoint = (vOwnerPos + vBezirPoint) / 2.f;
	//XMStoreFloat3(&m_CurvePoins[2], vCenterPoint);

	//_vector vCenterRightPoint = (vCenterPoint + vBezirPoint) / 2.f;
	//XMStoreFloat3(&m_CurvePoins[3], vCenterRightPoint);

	//_vector vCenterLeftPoint = (vOwnerPos + vCenterPoint) / 2.f;
	//XMStoreFloat3(&m_CurvePoins[1], vCenterLeftPoint);
	//XMStoreFloat3(&m_CurvePoins[0], vOwnerPos);


	CEffect::EFFECT_TRANSFORM_DESC EffectDesc;
	EffectDesc.fRotationPerSec = 1.f;
	EffectDesc.fSpeedPerSec = 1.f;

	EffectDesc.pRootMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	EffectDesc.vPos = XMVectorSet(0, 0, 0, 1);
	EffectDesc.fRot = _float3(0, 0, 0);
	EffectDesc.fSize = 0.175f;
	EffectDesc.iFloor = 2;
	m_pEffect = static_cast<CEffect*>(m_pGameInstance->Add_Get_GameObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Effect_Item_Aura"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"), &EffectDesc));

	m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) + XMVectorSet(0.f, 0.5f, 0.f, 0.f));

	if (FAILED(ADD_Components(*pDesc)))
		return E_FAIL;

	return S_OK;
}

void CItem::Priority_Update(_float fTimeDelta)
{
}

void CItem::Update(_float fTimeDelta)
{
	//if (m_bIsLerpAnimation)
	//{
	//	m_fLerpTime.x += fTimeDelta;
	//	m_pTransformCom->Set_State(STATE::POSITION, BezierCurve(5, m_CurvePoins, m_fLerpTime.x / m_fLerpTime.y));

	//	//if (m_fLerpTime.x >= 0.3f)
	//	//{
	//	//	m_pGameInstance->Add_RigidBody_ToPhysx(this, m_pRigidBody);
	//	//}
	//	if(m_fLerpTime.x > m_fLerpTime.y)
	//		m_bIsLerpAnimation = false;
	//}

	_matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	m_pCullingCollider->UpdateColiision(WorldMat);
	m_pRigidBody->Update_PxTransform(WorldMat);

	// ui 애니메이션 끝나고 나오게
	//if (m_eInterState == INTERACTION_STATE::ACTIVE)
	//{
	//	CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());

	//	if (pHUD)
	//	{
	//		//여기서 상호작용해서 나올거임
	//		if (pHUD->Check_AnimFinish(TEXT("Layer_World"), TEXT("SimpleKey_Cloned_0"), TEXT("Hide_Key_0")))
	//			m_pGameInstance->Remove_Interaction(m_pInteractionCom);
	//	}

	//	Safe_Release(pHUD);
	//}
}

void CItem::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	{
		m_pInteractionCom->Update_Com(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
#ifdef _DEBUG
		m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif

		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}

	m_pTrail->Update_Trail(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()), fTimeDelta * 0.5f, true);
}

HRESULT CItem::Render()
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

HRESULT CItem::Begin_OverlapCallBack()
{
	__super::Begin_OverlapCallBack();
	m_pInteractionCom->Set_InterState(INTERACTION_STATE::DEFAULT);

	return S_OK;
}

HRESULT CItem::End_OverlapCallBack()
{
	__super::End_OverlapCallBack();

	return S_OK;
}

void CItem::Excute_CallBack(_float fTimeDelta, CGameObject* pActionObject)
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
		CUIHUD* pHUD = dynamic_cast<CUIHUD*>(m_pGameInstance->GetCurrentLevelHUD());
		if (pHUD)
		{
			CUIGetterQueue* pGetterQueue{ dynamic_cast<CUIGetterQueue*>(pHUD->Get_UIObject(TEXT("Layer_Combat_Info"), TEXT("UI_GetterQueue"))) };
			if (pGetterQueue)
			{
				WCHAR pText[MAX_PATH] = {};
				wsprintf(pText, TEXT(" %d G"), (_int)m_fAmount);
				pGetterQueue->Insert_Queue(pText);

				auto pPlayer = CGameManager::GetInstance()->GetGameCharacter();

				if (pPlayer)
					static_cast<CPlayer*>(pPlayer)->Get_Desc()->iOwnGold += (_int)m_fAmount;

				Safe_Release(pPlayer);

				if (nullptr != m_pEffect) {
					m_pEffect->End(true, 1);
				}
			}
		}
		Safe_Release(pHUD);

		m_pInteractionCom->Set_InterState(INTERACTION_STATE::ACTIVE);
		m_pInteractionCom->Set_Duration(0.f);
		m_pGameInstance->Remove_Interaction(m_pInteractionCom);
		Set_Dead(true);
	}
}

HRESULT CItem::ADD_Components(const ACTOR_DESC& Desc)
{
	_float3 Com_Size = m_pTransformCom->Get_Scale();

	/* Com_Model */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), Desc.szVIBuffer_PrototypeName,
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Item"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Interaction */
	CInteractionBinder::INTERACTION_DESC InteractionDesc = {};
	InteractionDesc.vSize = Com_Size;
	InteractionDesc.BeginCallBackFunc = [&]() { this->Begin_OverlapCallBack(); };
	InteractionDesc.EndCallBackFunc = [&]() { this->End_OverlapCallBack(); };
	InteractionDesc.InteractionEvent = [&](_float fTimeDelta, CGameObject* pActionObject) { this->Excute_CallBack(fTimeDelta, pActionObject); };

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_InteractionBinder"),
		TEXT("Com_Interaction"), reinterpret_cast<CComponent**>(&m_pInteractionCom), &InteractionDesc)))
		return E_FAIL;

	m_pInteractionCom->Set_InterDesc(m_pGameManager->Find_InteractionData(0));
	m_pInteractionCom->SetInteractionHitType(HIT_TYPE::INTERACTION);
	m_pInteractionCom->ADD_InteractionIgnoreObject(HIT_TYPE::MONSTER);

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	PxUserData tUserData;
	// 밀려야하는 애들은 이키워드로 세팅
	tUserData.szActorTag = TEXT("Non_Collidable");

	//리지드 바디 Desc 세팅. 머테리얼이랑 Mass, userdata, shape, type 부분 위주로 살펴보세요.
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc;
	// 콜라이더 모양
	RigidBodyDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::BOX;

	// 충돌처리를 할지말지 
	// STATIC : 충돌하는데 가만히 있는 녀석
	// DYNAMIC : 충돌 
	// KINEMATIC : 충돌 X
	RigidBodyDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::DYNAMIC;

	RigidBodyDesc.StartWorldMatrix = *m_pTransformCom->Get_WorldMatrixPtr();
	RigidBodyDesc.tUserData = tUserData;
	RigidBodyDesc.vMaterial = _float3(0.f, 0.f, 0.f);
	RigidBodyDesc.vSize = _float3(Com_Size.x * 0.5f, Com_Size.y * 0.5f, Com_Size.z * 0.5f);
	RigidBodyDesc.fMass = { 0.1f };
	RigidBodyDesc.iCollisionGroup = PHYSX_CUSTOM_3;
	RigidBodyDesc.iCollisionMask  = PHYSX_TERRAIN | PHYSX_DEFAULT;
	RigidBodyDesc.isQuery = false;
	/* Com_RigidBody */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
		return E_FAIL;
	
	m_pGameInstance->Add_RigidBody_ToPhysx(this, m_pRigidBody);
	
	if(false == m_isHemiSphere)
	{
		// 아이템 데이터도 찾을거임 나중에 일단 잘 나오는지 보고 데이터 세팅하겠음
		_vector vDir = XMVector3Normalize(XMVectorSet(m_pGameInstance->Random_Normal() * 0.5f, m_pGameInstance->Random_Normal() + 0.8f, m_pGameInstance->Random_Normal() * 0.5f, 0.f));
		m_pRigidBody->Add_Impulse(vDir, 9.f, 6.f);
	}
	else if (true == m_isHemiSphere)
	{
		_vector vDir = XMVector3Normalize(XMVector3Rotate(XMVectorSetY(XMLoadFloat3(&m_vParentLook), 0.f), 
			XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pGameInstance->Random(-1.f * XM_PI / 2.f, XM_PI / 2.f))));

		vDir += XMVectorSet(0.f, 5.f, 0.f, 0.f);

		m_pRigidBody->Add_Impulse(vDir, 9.f, 6.f);
	}


	CTrailEffect::TRAIL_DATA Traildesc{};
	Traildesc.vHigh = _float4(0.03f, 0.f, 0.f, 0.f);
	Traildesc.vLow = _float4(-0.03f, 0.f, 0.f, 0.f);
	Traildesc.bisLine = false;
	Traildesc.bisLong = false;

	m_pTrail = static_cast<CTrailEffect*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_TrailEffect_Item_Trail"), &Traildesc));

	return S_OK;
}

HRESULT CItem::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CItem* CItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem* pItemData = new CItem(pDevice, pContext);
	if (FAILED(pItemData->Initialize_Prototype()))
	{
		Safe_Release(pItemData);
		MSG_BOX("Create Fail : Item Data");
	}
	return pItemData;
}

CGameObject* CItem::Clone(void* pArg)
{
	CItem* pItemData = new CItem(*this);
	if (FAILED(pItemData->Initialize(pArg)))
	{
		Safe_Release(pItemData);
		MSG_BOX("Clone Fail : Item Data");
	}
	return pItemData;
}

void CItem::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTrail);
}
