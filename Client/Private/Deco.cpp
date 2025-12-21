#include "pch.h"
#include "Deco.h"
#include "GameInstance.h"

CDeco::CDeco(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CDeco::CDeco(const CDeco& Prototype)
	: CActor{ Prototype }
{
}

HRESULT CDeco::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDeco::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ACTOR_DESC* pDesc = static_cast<ACTOR_DESC*>(pArg);

    SetCullingCollider(pDesc->iObjectID);
    _matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    m_pCullingCollider->UpdateColiision(worldMatrix);

	if (FAILED(Ready_Components(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;

	return S_OK;
}

void CDeco::Priority_Update(_float fTimeDelta)
{
}

void CDeco::Update(_float fTimeDelta)
{
}

void CDeco::Late_Update(_float fTimeDelta)
{
    if (!m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
    {
        return;
    }

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(RENDER::OCCLUSION, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
}

HRESULT CDeco::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
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

HRESULT CDeco::Ready_Components(const _tchar* pComponentTag)
{
    auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
    _float3 vColliderSize = pCullingCollider->GetBounding().Extents;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model_COL */
	//_wstring strComponentTag = pComponentTag;
	//strComponentTag += TEXT("_COL");


	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), strComponentTag,
	//	TEXT("Com_Model_COL"), reinterpret_cast<CComponent**>(&m_pColModelCom))))
	//	return E_FAIL;

	//// 충돌용 메시 피직스 세팅 조건
	//// 1. 충돌용 메시 생성 이후에 RigidBody를 세팅해주셔야합니다.
	//// 2. 해당 객체의 위치가 설정된 뒤에 RigidBody를 세팅해주셔야 합니다.

	//// 세팅 방법 보고도 잘 이해 안되면 물어봐주세요 키네마틱, 다이나믹, 스태틱 세팅 중요해요
	//PxUserData tUserData;
	//// 엘레베이터 식별용 문자열. 이건 나중에 엘베말고 다른데에 넣을떄 저랑 얘기하고 정해서 넣어주세요
	//tUserData.szActorTag = TEXT("Deco_Actor");

	//// 리지드 바디 Desc 세팅. 
	//// F12 타고 들어가서 머테리얼이랑 Mass, userdata, shape, type 부분 위주로 살펴보세요.
	//CRigidBody::RIGIDBODY_DESC RigidBodyDesc;
	//// 콜라이더 모양.
	//// ->> 메시는 TRIANGLE, 박스나 캡슐은 BOX, CAPSULE 다 따로 있으니까
	//// F12 타고 들어가서 한번 확인해보세요.
	//RigidBodyDesc.eRigidBodyShape = CRigidBody::RIGIDBODY_SHAPE::TRIANGLE;

	//// 충돌처리를 할지말지 
	//// DYNAMIC : 충돌 
	//// KINEMATIC : 충돌 X
	//// STATIC : 충돌 O, 대신 고정되어 있음.
	//// ->> 엘레베이터는 고정되어있으니까 STATIC으로 세팅 해주는거에요.

	//RigidBodyDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::STATIC;

	//RigidBodyDesc.StartWorldMatrix = *m_pTransformCom->Get_WorldMatrixPtr();
	//RigidBodyDesc.tUserData = tUserData;
	//RigidBodyDesc.vMaterial = _float3(0.5f, 0.5f, 0.3f);
	//RigidBodyDesc.vSize = m_pTransformCom->Get_Scale();
	//RigidBodyDesc.fMass = { 0.3f };
	//// TRIANGLE로 세팅하고 충돌용 메시 작업하는거니까, 충돌용 메시 넣어줘야돼요 
	//// TRIANGLE 타입이 아닌 (충돌용 메시가 아닌) 녀석들은 굳이 안넣어줘도 됩니다.
	//RigidBodyDesc.pColModel = m_pColModelCom;

	///* Com_RigidBody */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
	//	TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	//	return E_FAIL;

	//// 리지드 바디 세팅 끝났으면 Physx 매니저에 집어넣는 과정도 있어야돼요.
	//// 없으면 충돌 안됨
	//m_pGameInstance->Add_RigidBody_ToPhysx(this, m_pRigidBody);

	/*_float3 Com_Size = m_pTransformCom->Get_Scale();
	Com_Size.x *= 2.f;
	Com_Size.z *= 2.f;

	static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision({}, {}, Com_Size);*/

	return S_OK;
}

HRESULT CDeco::Bind_ShaderResources()
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

void CDeco::SetCullingCollider(_uint iObjectID)
{
    auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
    switch (iObjectID)
    {
    case 80: // Lamp_47A
        pCullingCollider->SetCollision({ 0.f, 4.f, 1.f }, {}, { 2.f, 4.f, 2.f });
        break;
    case 81: // Poster_4D
        pCullingCollider->SetCollision({ 0.f, -2.f, 0.f }, {}, { 2.f, 2.5f, 1.f });
        break;
    case 82: // Wheel_1A
    case 83: // Wheel_1B
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.5f }, {}, { 3.f, 2.f, 3.f });
        break;
    case 84: // Wheel_1C
        pCullingCollider->SetCollision({ 0.f, 3.5f, 0.f }, {}, { 3.f, 4.f, 3.f });
        break;
    case 85: // Base_1A
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 10.f, 2.f, 10.f });
        break;
    case 86: // Camp_1B
        pCullingCollider->SetCollision({ 0.f, 0.7f, 0.f }, {}, { 2.f, 1.f, 2.f });
        break;
    case 87: // Camp_1D
        pCullingCollider->SetCollision({ 0.5f, 1.4f, 0.f }, {}, { 1.2f, 2.f, 1.2f });
        break;
    case 88: // Camp_1E
        pCullingCollider->SetCollision({ 0.f, 1.05f, 0.f }, {}, { 1.1f, 1.5f, 1.1f });
        break;
    case 89: // Camp_1F
        pCullingCollider->SetCollision({ 0.f, 1.f, 0.f }, {}, { 1.f, 1.f, 1.f });
        break;
    case 90: // Camp_1G
        pCullingCollider->SetCollision({ 0.f, 1.3f, 0.f }, {}, { 1.f, 1.3f, 1.f });
        break;
    case 91: // Camp_1H
        pCullingCollider->SetCollision({ 0.f, 0.5f, 0.f }, {}, { 0.8f, 0.5f, 0.8f });
        break;
    case 92: // Camp_1J
        pCullingCollider->SetCollision({ 0.f, 0.49f, 0.f }, {}, { 1.f, 0.7f, 1.5f });
        break;
    case 93: // Camp_1K
        pCullingCollider->SetCollision({ 0.f, 0.56f, 0.f }, {}, { 1.f, 0.8f, 1.8f });
        break;
    case 94: // Camp_1L
        pCullingCollider->SetCollision({ 0.f, 0.35f, 0.f }, {}, { 0.8f, 0.5f, 0.8f });
        break;
    case 95: // Camp_1N
        pCullingCollider->SetCollision({ 0.f, 0.5f, 0.f }, {}, { 0.5f, 0.5f, 0.5f });
        break;
    case 96: // Camp_1R
        pCullingCollider->SetCollision({ 0.f, 0.49f, 0.f }, {}, { 0.7f, 0.7f, 0.7f });
        break;
    case 97: // Camp_1S
        pCullingCollider->SetCollision({ 0.f, 0.7f, 0.f }, {}, { 0.7f, 1.f, 0.7f });
        break;
    case 98: // Camp_1T
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 1.f, 2.f, 1.f });
        break;
    case 99: // Container_2A
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 3.f, 3.f, 5.f });
        break;
    case 100: // Container_2B
    case 102: // Container_2D
        pCullingCollider->SetCollision({ 0.f, 1.05f, 0.f }, {}, { 2.f, 1.5f, 2.f });
        break;
    case 101: // Container_2C
    case 103: // Container_3A
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 2.f, 2.f, 2.f });
        break;
    case 104: // Container_4B
        pCullingCollider->SetCollision({ 0.f, 6.f, 0.f }, {}, { 4.f, 7.f, 4.f });
        break;
    case 105: // Container_5A
    case 107: // Container_5C
    case 108: // Container_5D
        pCullingCollider->SetCollision({ 0.f, 2.8f, 0.f }, {}, { 4.f, 4.f, 8.f });
        break;
    case 106: // Container_5B
        pCullingCollider->SetCollision({ 0.f, 2.8f, 0.f }, {}, { 6.f, 4.f, 8.f });
        break;
    case 109: // Container_5E
        pCullingCollider->SetCollision({ 1.f, 2.1f, 0.f }, {}, { 2.f, 3.f, 1.f });
        break;
    case 110: // Container_7B
        pCullingCollider->SetCollision({ 0.f, 8.f, 0.f }, {}, { 10.f, 2.f, 20.f });
        break;
    case 111: // Container_7F
        pCullingCollider->SetCollision({ 0.f, 7.f, 0.f }, {}, { 12.f, 10.f, 25.f });
        break;
    case 112: // Fence_1A
    case 113: // Fence_1B
    case 114: // Fence_1F
    case 115: // Fence_1H
        pCullingCollider->SetCollision({ -3.f, 3.5f, 0.f }, {}, { 5.f, 4.f, 1.f });
        break;
    case 116: // Vehicle_2A
        pCullingCollider->SetCollision({ -2.f, 3.5f, 0.f }, {}, { 7.f, 4.f, 5.f });
        break;
    case 117: // Vehicle_2B
        pCullingCollider->SetCollision({ 0.f, 4.5f, 0.f }, {}, { 12.f, 5.f, 5.f });
        break;
    case 118: // Vehicle_3B
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 3.f, 3.f, 7.f });
        break;
    case 119: // Vehicle_4C
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 7.f, 3.f, 4.f });
        break;
    case 120: // Vehicle_6A
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 4.f, 3.f, 7.f });
        break;
    case 121: // Vehicle_8B
        pCullingCollider->SetCollision({ 0.f, 3.5f, 0.f }, {}, { 15.f, 5.f, 5.f });
        break;
    case 122: // Vehicle_14A
    case 123: // Vehicle_14B
        pCullingCollider->SetCollision({ 0.f, 3.5f, 0.f }, {}, { 12.f, 5.f, 6.f });
        break;
    case 124: // Sign_11A
        pCullingCollider->SetCollision({ 0.f, 7.f, 0.f }, {}, { 6.f, 10.f, 4.f });
        break;
    case 125: // Sign_11B
        pCullingCollider->SetCollision({ 0.f, 7.f, 0.f }, {}, { 10.f, 10.f, 4.f });
        break;
    case 126: // Sign_11F
        pCullingCollider->SetCollision({ 0.f, 4.2f, 0.f }, {}, { 15.f, 6.f, 4.f });
        break;
    case 127: // Sign_12B
        pCullingCollider->SetCollision({ 0.f, 0.f, 0.f }, {}, { 4.f, 2.f, 2.f });
        break;
    case 128: // Sign_12C
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 2.f, 3.f, 2.f });
        break;
    case 129: // Sign_36B
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 4.f, 2.f, 1.f });
        break;
    case 130: // Crane_1C
        pCullingCollider->SetCollision({ -1.f, -5.f, 0.f }, {}, { 4.f, 7.f, 2.f });
        break;
    case 131: // Crane_11
        pCullingCollider->SetCollision({ 0.f, 35.f, 20.f }, {}, { 15.f, 40.f, 55.f });
        break;
    case 132: // Crane_13
        pCullingCollider->SetCollision({ 0.f, 22.f, 10.f }, {}, { 10.f, 28.f, 20.f });
        break;
    case 133: // Trash_1A
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 4.f, 2.f, 3.f });
        break;
    case 134: // Trash_2A
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 5.f, 2.f, 3.f });
        break;
    case 135: // Trash_2B
        pCullingCollider->SetCollision({ 0.f, 2.1f, 0.f }, {}, { 6.f, 3.f, 3.f });
        break;
    case 136: // Trash_4A
        pCullingCollider->SetCollision({ 0.f, 0.7f, 0.f }, {}, { 2.f, 1.f, 2.f });
        break;
    case 137: // Trash_9A
        pCullingCollider->SetCollision({ 2.f, -2.f, -2.f }, {}, { 4.f, 3.f, 4.f });
        break;
    case 138: // Trash_17A
        pCullingCollider->SetCollision({ 0.f, 1.4f, 0.f }, {}, { 4.f, 2.f, 4.f });
        break;
    }
}


CDeco* CDeco::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDeco* pInstance = new CDeco(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDeco::Clone(void* pArg)
{
	CDeco* pInstance = new CDeco(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDeco");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDeco::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
