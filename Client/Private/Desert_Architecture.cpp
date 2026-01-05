#include "pch.h"
#include "Desert_Architecture.h"
#include "GameInstance.h"

CDesert_Architecture::CDesert_Architecture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CDesert_Architecture::CDesert_Architecture(const CDesert_Architecture& Prototype)
	: CActor{ Prototype }
{
}

HRESULT CDesert_Architecture::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDesert_Architecture::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ACTOR_DESC* pDesc = static_cast<ACTOR_DESC*>(pArg);

	SetCullingCollider(pDesc->iObjectID);

	_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	m_pCullingCollider->UpdateColiision(worldMatrix);

	if (FAILED(Ready_Components(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;

	if (wcsstr(pDesc->szVIBuffer_PrototypeName, TEXT("Bridge")))
	{
		if (FAILED(Ready_Col(pDesc->szVIBuffer_PrototypeName)))
			return S_OK;
	}
		

	return S_OK;
}

void CDesert_Architecture::Priority_Update(_float fTimeDelta)
{
}

void CDesert_Architecture::Update(_float fTimeDelta)
{
	
}

void CDesert_Architecture::Late_Update(_float fTimeDelta)
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

HRESULT CDesert_Architecture::Render()
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

HRESULT CDesert_Architecture::Ready_Components(const _tchar* pComponentTag)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDesert_Architecture::Ready_Col(const _tchar* pComponentTag)
{
	/* Com_Model_COL */
	_wstring strComponentTag = pComponentTag;
	strComponentTag += TEXT("_COL");

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LEVEL_PROB), strComponentTag,
		TEXT("Com_Model_COL"), reinterpret_cast<CComponent**>(&m_pColModelCom))))
		return E_FAIL;

	// 충돌용 메시 피직스 세팅 조건
	// 1. 충돌용 메시 생성 이후에 RigidBody를 세팅해주셔야합니다.
	// 2. 해당 객체의 위치가 설정된 뒤에 RigidBody를 세팅해주셔야 합니다.

	// 세팅 방법 보고도 잘 이해 안되면 물어봐주세요 키네마틱, 다이나믹, 스태틱 세팅 중요해요
	PxUserData tUserData;
	// 엘레베이터 식별용 문자열. 이건 나중에 엘베말고 다른데에 넣을떄 저랑 얘기하고 정해서 넣어주세요
	tUserData.szActorTag = TEXT("Architecture_Actor");

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

HRESULT CDesert_Architecture::Bind_ShaderResources()
{
	_bool bFlag = { false };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthB", &bFlag, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMaskingDepthW", &bFlag, sizeof(_bool))))
		return E_FAIL;

	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

void CDesert_Architecture::SetCullingCollider(_uint iObjectID)
{
	auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
	switch (iObjectID)
	{
	case 1: // Ruin_7A (20, 30, 35)
		pCullingCollider->SetCollision({ -5.f, 21.f, -15.f }, {}, { 20.f, 30.f, 35.f });
		break;
	case 2: // Ruin_7B (30, 15, 25)
		pCullingCollider->SetCollision({ -20.f, 10.5f, -10.f }, {}, { 30.f, 15.f, 25.f });
		break;
	case 3: // Ruin_7C (12, 25, 12)
		pCullingCollider->SetCollision({ 0.f, 23.f, 0.f }, {}, { 12.f, 25.f, 12.f });
		break;
	case 4: // Ruin_7D (18, 8, 5)
		pCullingCollider->SetCollision({ 0.f, 4.f, 0.f }, {}, { 18.f, 8.f, 5.f });
		break;
	case 5: // Ruin_7E (4, 6, 4)
		pCullingCollider->SetCollision({ 0.f, 4.2f, 0.f }, {}, { 4.f, 6.f, 4.f });
		break;
	case 6: // Bridge_4A (10, 65, 30)
		pCullingCollider->SetCollision({ 0.f, 40.f, 0.f }, {}, { 10.f, 65.f, 30.f });
		break;
	case 7: // Bridge_4B (50, 65, 30)
		pCullingCollider->SetCollision({ 30.f, 45.5f, 0.f }, {}, { 40.f, 65.f, 30.f });
		break;
	case 8: // Bridge_4D (30, 80, 30)
		pCullingCollider->SetCollision({ 0.f, 45.f, 0.f }, {}, { 30.f, 80.f, 30.f });
		break;
	case 9: // Bridge_4L (40, 120, 150)
		pCullingCollider->SetCollision({ -10.f, -90.f, 100.f }, {}, { 40.f, 120.f, 150.f });
		break;
	case 10: // Bridge_4M (70, 70, 80)
		pCullingCollider->SetCollision({ 0.f, 30.f, -100.f }, {}, { 70.f, 80.f, 80.f });
		break;
	case 11: // Bridge_4N (30, 10, 5)
		pCullingCollider->SetCollision({ -35.f, 40.f, -70.f }, {}, { 30.f, 10.f, 5.f });
		break;
	case 12: // Bridge_5 (50, 60, 50)
		pCullingCollider->SetCollision({ 0.f, -10.f, 0.f }, {}, { 50.f, 60.f, 50.f });
		break;
	case 13: // Bridge_6 (60, 110, 100)
		pCullingCollider->SetCollision({ 0.f, 65.f, 0.f }, {}, { 60.f, 110.f, 100.f });
		break;
	case 14: // Bridge_14A (6, 8, 25)
		pCullingCollider->SetCollision({ 0.f, 5.6f, 0.f }, {}, { 6.f, 8.f, 25.f });
		break;
	case 15: // Bridge_14B (6, 5, 20)
		pCullingCollider->SetCollision({ 0.f, 3.5f, 0.f }, {}, { 6.f, 5.f, 20.f });
		break;
	case 16: // Ruin_A
		pCullingCollider->SetCollision({ -3.f, 5.0f, 0.f }, {}, { 10.f, 6.f, 6.f });
		break;
	case 17: // Ruin_B
		pCullingCollider->SetCollision({ 0.f, 10.f, 0.f }, {}, { 7.f, 11.f, 7.f });
		break;
	case 18: // Ruin_C
		pCullingCollider->SetCollision({ 4.f, 5.0f, -1.f }, {}, { 6.f, 6.f, 6.f });
		break;
	case 19: // Ruin_D
		pCullingCollider->SetCollision({ -0.5f, 6.0f, 1.5f }, {}, { 8.f, 8.f, 8.f });
		break;
	case 20: // Ruin_Building_A
		pCullingCollider->SetCollision({ 0.f, 28.f, -1.5f }, {}, { 20.f, 33.f, 20.f });
		break;
	case 21: // Ruin_Building_B
		pCullingCollider->SetCollision({ 0.f, 21.f, 0.f }, {}, { 22.f, 22.f, 28.f });
		break;
	case 22: // Ruin_Building_C
		pCullingCollider->SetCollision({ 10.f, 16.0f, 5.f }, {}, { 30.f, 20.f, 35.f });
		break;
	case 23: // Floor_A
		pCullingCollider->SetCollision({ 0.f, 0.f, 0.f }, {}, { 6.f, 1.f, 6.f });
		break;
	case 24: // Floor_B
		pCullingCollider->SetCollision({ -3.f, 0.5f, 0.f }, {}, { 7.f, 5.f, 8.f });
		break;
	case 25: // Floor_C
		pCullingCollider->SetCollision({ 0.f, -0.2f, 0.f }, {}, { 5.f, 2.f, 3.f });
		break;
	case 26: // Floor_D
		pCullingCollider->SetCollision({ 0.f, 1.0f, 0.f }, {}, { 8.f, 2.f, 8.f });
		break;
	case 27: // Floor_E
		pCullingCollider->SetCollision({ 0.f, 0.5f, 0.f }, {}, { 7.f, 2.f, 7.f });
		break;
	case 28: // Floor_F
		pCullingCollider->SetCollision({ -1.f, 0.2f, 0.f }, {}, { 2.f, 1.f, 6.f });
		break;
	case 29: // Floor_G
		pCullingCollider->SetCollision({ 0.f, 0.2f, 0.f }, {}, { 7.f, 1.f, 7.f });
		break;
	case 30: // Floor_H
		pCullingCollider->SetCollision({ 0.f, 0.5f, 0.f }, {}, { 8.f, 2.f, 8.f });
		break;
	case 31: // Frame_A
		pCullingCollider->SetCollision({ 0.f, 9.f, -1.5f }, {}, { 1.5f, 16.f, 13.f });
		break;
	case 32: // Frame_B
		pCullingCollider->SetCollision({ 0.f, -1.f, 0.f }, {}, { 2.f, 8.f, 7.f });
		break;
	case 33: // Stone009_A
		pCullingCollider->SetCollision({ 0.f, -0.5f, -5.5f }, {}, { 1.f, 1.f, 6.f });
		break;
	case 34: // Stone009_B
		pCullingCollider->SetCollision({ 1.f, 0.5f, -2.7f }, {}, { 2.f, 1.f, 4.f });
		break;
	case 35: // Wall_A
		pCullingCollider->SetCollision({ 0.f, 6.f, 0.f }, {}, { 8.f, 7.f, 2.f });
		break;
	case 36: // Wall_B
		pCullingCollider->SetCollision({ -1.f, 6.f, 0.f }, {}, { 10.f, 7.f, 2.f });
		break;
	case 37: // Wall_C
		pCullingCollider->SetCollision({ 0.5f, 6.f, 0.f }, {}, { 7.f, 7.f, 2.f });
		break;
	case 38: // Wall_D
		pCullingCollider->SetCollision({ 0.f, 3.0f, 0.f }, {}, { 9.f, 4.f, 2.f });
		break;
	case 39: // Wall_E
		pCullingCollider->SetCollision({ 0.f, 3.0f, 0.f }, {}, { 10.f, 4.f, 2.f });
		break;
	case 40: // Wall_F
		pCullingCollider->SetCollision({ 1.f, 2.5f, 0.f }, {}, { 9.f, 4.f, 2.f });
		break;
	case 41: // Wall_G
		pCullingCollider->SetCollision({ -0.5f, 2.f, 0.f }, {}, { 10.f, 3.f, 2.f });
		break;
	case 42: // Wall_H
		pCullingCollider->SetCollision({ 3.f, -1.f, 0.f }, {}, { 11.f, 3.f, 2.f });
		break;
	case 43: // Wall_I
		pCullingCollider->SetCollision({ -3.f, -1.f, 0.f }, {}, { 7.f, 4.f, 2.f });
		break;
	case 44: // Wall007_A
		pCullingCollider->SetCollision({ 3.5f, 4.5f, 0.f }, {}, { 5.f, 6.f, 2.f });
		break;
	case 45: // Wall007_B
		pCullingCollider->SetCollision({ -0.5f, 3.5f, 0.f }, {}, { 8.f, 5.f, 2.f });
		break;
	case 46: // Wall007_C
		pCullingCollider->SetCollision({ 0.f, 5.0f, 0.f }, {}, { 5.f, 6.f, 2.f });
		break;
	case 47: // Wall007_D
		pCullingCollider->SetCollision({ 0.f, 5.0f, 0.f }, {}, { 8.f, 6.f, 2.f });
		break;
	case 48: // Wall007_E
		pCullingCollider->SetCollision({ -0.5f, 4.f, 0.f }, {}, { 8.f, 5.f, 2.f });
		break;
	case 49: // Wall007_F
		pCullingCollider->SetCollision({ -0.5f, 2.f, 0.f }, {}, { 8.f, 3.f, 2.f });
		break;
	case 50: // Wall007_G
		pCullingCollider->SetCollision({ -0.5f, 3.5f, 0.f }, {}, { 8.f, 5.f, 2.f });
		break;
	case 51: // Xion_Wall_1C (40, 20, 40)
		pCullingCollider->SetCollision({ -36.f, 19.0f, 37.f }, {}, { 40.f, 20.f, 40.f });
		break;
	case 52: // Xion_Wall_1D (40, 20, 40)
		pCullingCollider->SetCollision({ -36.f, 19.0f, -37.f }, {}, { 40.f, 20.f, 40.f });
		break;
	case 53: // Xion_Wall_1Db (40, 20, 40)
		pCullingCollider->SetCollision({ -36.f, 19.0f, -37.f }, {}, { 40.f, 20.f, 40.f });
		break;
	case 54: // Xion_Wall_1G (10, 43, 10)
		pCullingCollider->SetCollision({ 0.f, 42.f, 0.f }, {}, { 10.f, 43.f, 10.f });
		break;
	case 55: // Xion_Wall_1I (35, 60, 62)
		pCullingCollider->SetCollision({ -30.5f, 59.0f, -58.f }, {}, { 35.f, 60.f, 62.f });
		break;
	case 56: // Xion_Wall_1K (25, 60, 8)
		pCullingCollider->SetCollision({ 0.f, 59.0f, -5.5f }, {}, { 25.f, 60.f, 8.f });
		break;
	case 57: // Xion_Wall_1Q (35, 23, 35)
		pCullingCollider->SetCollision({ -20.f, -19.f, -25.f }, {}, { 35.f, 23.f, 35.f });
		break;
	case 58: // Xion_Wall_1R (20, 21, 12)
		pCullingCollider->SetCollision({ -15.f, -19.f, -5.f }, {}, { 20.f, 21.f, 12.f });
		break;
	case 59: // Xion_Wall_1S (10, 22, 17)
		pCullingCollider->SetCollision({ -14.f, -19.5f, -11.f }, {}, { 15.f, 21.f, 17.f });
		break;
	case 60: // Xion_Wall_1T (10, 22, 23)
		pCullingCollider->SetCollision({ 6.f, -21.f, -19.f }, {}, { 10.f, 22.f, 23.f });
		break;
	case 61: // Xion_Wall_2A (85, 36, 9)
		pCullingCollider->SetCollision({ -80.f, -34.0f, -5.f }, {}, { 84.f, 36.f, 9.f });
		break;
	case 62: // Xion_Wall_2B (85, 36, 9)
		pCullingCollider->SetCollision({ -80.f, -34.0f, -5.f }, {}, { 84.f, 36.f, 9.f });
		break;
	case 63: // Xion_Wall_4H (25, 35, 13)
		pCullingCollider->SetCollision({ 0.f, 33.f, -2.f }, {}, { 25.f, 35.f, 13.f });
		break;
	case 64: // Xion_Wall_5A (20, 43, 11)
		pCullingCollider->SetCollision({ 0.f, 41.5f, 0.f }, {}, { 20.f, 43.f, 11.f });
		break;
	case 65: // Xion_Wall_8A (32, 17, 4)
		pCullingCollider->SetCollision({ 30.f, 15.f, 1.f }, {}, { 32.f, 17.f, 4.f });
		break;
	case 66: // Xion_Wall_9A (25, 13, 3)
		pCullingCollider->SetCollision({ -22.f, 11.f, -1.f }, {}, { 25.f, 13.f, 3.f });
		break;
	case 67: // Xion_Wall_9B (10, 12, 2)
		pCullingCollider->SetCollision({ -8.f, 11.0f, -0.5f }, {}, { 10.f, 12.f, 2.f });
		break;
	case 68: // Xion_Wall_10A (6, 17, 32)
		pCullingCollider->SetCollision({ 0.f, 15.5f, 30.f }, {}, { 6.f, 17.f, 32.f });
		break;
	case 69: // Xion_Wall_10B (5, 17, 32)
		pCullingCollider->SetCollision({ 1.f, 15.5f, 30.f }, {}, { 5.f, 17.f, 32.f });
		break;
	case 70: // Xion_Wall_10C (1.5, 3, 7)
		pCullingCollider->SetCollision({ 0.f, 2.5f, 5.8f }, {}, { 1.5f, 3.f, 6.5f });
		break;
	case 71: // Xion_Wall_10D (6, 2.5, 4)
		pCullingCollider->SetCollision({ -5.f, 2.f, -3.f }, {}, { 6.f, 2.5f, 4.f });
		break;
	case 72: // Xion_Building_3A (19, 28, 13) 
		pCullingCollider->SetCollision({ 0.f, 26.0f, 0.f }, {}, { 19.f, 28.f, 13.f });
		break;
	case 73: // Xion_Building_4A (30, 42, 38)   TODO
		pCullingCollider->SetCollision({ 0.f, 40.0f, 0.f }, {}, { 30.f, 42.f, 38.f });
		break;
	case 74: // Xion_Building_5A (42, 44, 40)
		pCullingCollider->SetCollision({ 0.f, 42.0f, -20.f }, {}, { 42.f, 44.f, 40.f });
		break;
	case 75: // Xion_Building_6A (58, 70, 40)
		pCullingCollider->SetCollision({ 7.f, 68.0f, -10.f }, {}, { 58.f, 70.f, 40.f });
		break;
	case 76: // Xion_Building_8 (65, 70, 60)
		pCullingCollider->SetCollision({ 58.f, 35.0f, -40.f }, {}, { 65.f, 70.f, 60.f });
		break;
	case 77: // Xion_Building_9A (15, 7, 10)
		pCullingCollider->SetCollision({ 26.f, 5.f, 19.f }, {}, { 15.f, 7.f, 10.f });
		break;
	case 78: // Xion_Building_17 (80, 80, 85)
		pCullingCollider->SetCollision({ 0.f, -70.0f, 30.f }, {}, { 80.f, 80.f, 85.f });
		break;
	case 79: // Xion_Building4_1A (95, 80, 40)
		pCullingCollider->SetCollision({ -13.f, 15.0f, 0.f }, {}, { 95.f, 80.f, 40.f });
		break;
	case 80: // Xion_Building4_1B (88, 93, 40)
		pCullingCollider->SetCollision({ -20.f, 5.f, 6.f }, {}, { 88.f, 93.f, 40.f });
		break;
	case 81: // Xion_Building4_1C (120, 120, 60)
		pCullingCollider->SetCollision({ 0.f, 57.0f, 27.f }, {}, { 120.f, 120.f, 60.f });
		break;
	case 82: // Xion_Building4_1D (70, 86, 45)
		pCullingCollider->SetCollision({ 10.f, 73.0f, 10.f }, {}, { 70.f, 86.f, 45.f });
		break;
	case 83: // Xion_Building4_1E (60, 95, 50)
		pCullingCollider->SetCollision({ 3.f, 93.f, 10.f }, {}, { 60.f, 95.f, 55.f });
		break;
	case 84: // Xion_Building4_1F (60, 78, 40)
		pCullingCollider->SetCollision({ 10.f, 75.0f, 8.f }, {}, { 60.f, 78.f, 40.f });
		break;
	case 85: // Xion_Building5_4 (35, 45, 40)
		pCullingCollider->SetCollision({ 0.f, 43.f, 0.f }, {}, { 35.f, 46.f, 40.f });
		break;
	case 86: // Xion_Building5_callD4 (30, 42, 40)
		pCullingCollider->SetCollision({ 0.f, 40.0f, 0.f }, {}, { 30.f, 42.f, 40.f });
		break;
	case 87: // Xion_Building5_callD5 (40, 38, 40)
		pCullingCollider->SetCollision({ 0.f, 36.0f, -20.f }, {}, { 40.f, 38.f, 40.f });
		break;
	case 88: // Store_3 (10, 6, 10)
		pCullingCollider->SetCollision({ 8.f, 5.0f, 11.5f }, {}, { 10.f, 6.f, 13.f });
		break;
	case 89: // Store_4 (10, 6, 7)
		pCullingCollider->SetCollision({ 9.f, 5.0f, 6.f }, {}, { 10.f, 6.f, 7.f });
		break;
	case 90: // Stair_1A (14, 4, 8)
		pCullingCollider->SetCollision({ -14.f, 6.5f, 13.f }, {}, { 14.f, 4.f, 8.f });
		break;
	case 91: // Stair_1B (14, 4, 8)
		pCullingCollider->SetCollision({ -14.5f, 13.f, -18.f }, {}, { 14.f, 4.f, 8.f });
		break;
	case 92: // Stair_1C (31, 2, 17)
		pCullingCollider->SetCollision({ 0.f, 1.3f, 31.f }, {}, { 31.f, 2.f, 17.f });
		break;
	case 93: // Stair_2A (7, 2, 5)
		pCullingCollider->SetCollision({ 0.f, 1.0f, -2.f }, {}, { 7.f, 2.f, 5.f });
		break;
	case 94: // Stair_3A (5, 4, 5)
		pCullingCollider->SetCollision({ 0.f, 3.0f, -4.f }, {}, { 5.f, 4.f, 5.f });
		break;
	case 95: // Stair_3B (4.5, 1.5, 1.5)
		pCullingCollider->SetCollision({ 0.f, 0.75f, -1.f }, {}, { 4.5f, 1.5f, 1.5f });
		break;
	default:
		break;
	}

}

pair<wstring, wstring> CDesert_Architecture::Extract_ComponentTag(const _tchar* pComponentTag)
{
	wstring strComponentTag = pComponentTag;

	vector<wstring> tokens;
	wstring token;
	wstringstream tokenStream(strComponentTag);

	while (getline(tokenStream, token, L'_'))
	{
		tokens.push_back(token);
	}

	if (token.size() >= 2)
	{
		wstring tagCategoryPart = tokens[token.size() - 2];
		wstring numberPart = tokens[token.size() - 1];
		return { tagCategoryPart, numberPart };
	}

	return { L"", L"" };
}


CDesert_Architecture* CDesert_Architecture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDesert_Architecture* pInstance = new CDesert_Architecture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDesert_Architecture::Clone(void* pArg)
{
	CDesert_Architecture* pInstance = new CDesert_Architecture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDesert_Architecture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDesert_Architecture::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
