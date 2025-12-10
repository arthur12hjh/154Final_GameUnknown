#include "pch.h"
#include "Item.h"

#include "GameInstance.h"
#include "Interaction_Component.h"

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
	m_iInterID = 0;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ITEM_DESC* pDesc = static_cast<ITEM_DESC*>(pArg);
	m_bIsLerpAnimation = true;
	m_fAmount = pDesc->fAmount;

	_vector vOwnerPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vDropPoint = XMLoadFloat3(&pDesc->fDropPoint);

	_vector vBezirPoint = XMVectorLerp(vOwnerPos, vDropPoint, 1.f);
	vBezirPoint.m128_f32[1] += 5.f;
	XMStoreFloat3(&m_CurvePoins[4], vDropPoint);

	_vector vCenterPoint = (vOwnerPos + vBezirPoint) / 2.f;
	XMStoreFloat3(&m_CurvePoins[2], vCenterPoint);

	_vector vCenterRightPoint = (vCenterPoint + vBezirPoint) / 2.f;
	XMStoreFloat3(&m_CurvePoins[3], vCenterRightPoint);

	_vector vCenterLeftPoint = (vOwnerPos + vCenterPoint) / 2.f;
	XMStoreFloat3(&m_CurvePoins[1], vCenterLeftPoint);
	XMStoreFloat3(&m_CurvePoins[0], vOwnerPos);

	if (FAILED(ADD_Components(*pDesc)))
		return E_FAIL;

	// 아이템 데이터도 찾을거임 나중에 일단 잘 나오는지 보고 데이터 세팅하겠음


	return S_OK;
}

void CItem::Priority_Update(_float fTimeDelta)
{
}

void CItem::Update(_float fTimeDelta)
{
	if (m_bIsLerpAnimation)
	{
		m_fLerpTime.x += fTimeDelta;
		m_pTransformCom->Set_State(STATE::POSITION, BezierCurve(5, m_CurvePoins, m_fLerpTime.x / m_fLerpTime.y));

		if (m_fLerpTime.x >= 0.3f)
		{
			m_pGameInstance->Add_RigidBody_ToPhysx(this, m_pRigidBody);
		}
		if(m_fLerpTime.x > m_fLerpTime.y)
			m_bIsLerpAnimation = false;
	}

	_matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	m_pCullingCollider->UpdateColiision(WorldMat);
	m_pRigidBody->Update_PxTransform(WorldMat);
}

void CItem::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	{
		m_pInteractionCom->Update_Com();
#ifdef _DEBUG
		m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
		m_pGameInstance->Add_PhysxGeometry(m_pRigidBody->Get_PxRigidBody(), m_pRigidBody->Get_PxShape());
#endif

		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
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
	m_pGameInstance->ADD_Interaction(m_pInteractionCom);
	m_eInterState = INTERACTION_STATE::DEFAULT;

	return S_OK;
}

HRESULT CItem::End_OverlapCallBack()
{
	m_pGameInstance->Remove_Interaction(m_pInteractionCom);
	m_eInterState = INTERACTION_STATE::END;

	return S_OK;
}

void CItem::Excute_CallBack(CGameObject* pActionObject)
{
	m_eInterState = INTERACTION_STATE::ACTIVE;

	// 여기서 상호작용해서 나올거임
	m_pGameInstance->Remove_Interaction(m_pInteractionCom);
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
	CInteraction_Component::INTERACTION_DESC InteractionDesc = {};
	InteractionDesc.vSize = Com_Size;
	InteractionDesc.BeginCallBackFunc = [&]() { this->Begin_OverlapCallBack(); };
	InteractionDesc.EndCallBackFunc = [&]() { this->End_OverlapCallBack(); };
	InteractionDesc.InteractionEvent = [&](CGameObject* pActionObject) { this->Excute_CallBack(pActionObject); };

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Interaction"),
		TEXT("Com_Interaction"), reinterpret_cast<CComponent**>(&m_pInteractionCom), &InteractionDesc)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	PxUserData tUserData;
	// 밀려야하는 애들은 이키워드로 세팅
	tUserData.szActorTag = TEXT("Item_Actor");

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
	RigidBodyDesc.vMaterial = _float3(0.5f, 0.5f, 0.3f);
	RigidBodyDesc.vSize = Com_Size;
	RigidBodyDesc.fMass = { 0.3f };

	/* Com_RigidBody */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
		return E_FAIL;


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
}
