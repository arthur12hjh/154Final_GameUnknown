#include "pch.h"
#include "Canyon.h"
#include "GameInstance.h"
#include "Camera.h"

CCanyon::CCanyon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CCanyon::CCanyon(const CCanyon& Prototype)
	: CActor{ Prototype }
{
}

HRESULT CCanyon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCanyon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ACTOR_DESC* pDesc = static_cast<ACTOR_DESC*>(pArg);
    SetCullingCollider(pDesc->iObjectID);

	_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	m_pCullingCollider->UpdateColiision(worldMatrix);
    

	if (FAILED(Ready_Components(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;

	m_iObjectID = pDesc->iObjectID;

	if (m_iObjectID != 67 && m_iObjectID != 68 && m_iObjectID != 69 && m_iObjectID != 70 && m_iObjectID != 71 && m_iObjectID != 72
		&& m_iObjectID != 73 && m_iObjectID != 74 && m_iObjectID != 76 && m_iObjectID != 77 && m_iObjectID != 78 && m_iObjectID != 79)
	{
		if (FAILED(Ready_Col(pDesc->szVIBuffer_PrototypeName)))
			return E_FAIL;
	}

	m_pGameInstance->Add_StaticShadowObject(this); 
    //m_pRigidBody->Update_PxTransform(worldMatrix);
	return S_OK;
}

void CCanyon::Priority_Update(_float fTimeDelta)
{

}

void CCanyon::Update(_float fTimeDelta)
{


}

void CCanyon::Late_Update(_float fTimeDelta)
{
	/*if (!m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	{
		m_bVisible = false;
		return; 
	}*/

	if (GetVisibility() == VISIBILITY::HIDDEN)
		SetVisibility(VISIBILITY::VISIBLE);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

	//m_pGameInstance->Add_RenderGroup(RENDER::OCCLUSION, this);

	/*m_pGameInstance->Add_RenderGroup(RENDER::OCCLUSION, this);

	if (GetVisibility() == VISIBILITY::VISIBLE)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}*/


#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
#endif
}

HRESULT CCanyon::Render()
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

HRESULT CCanyon::Render_Shadow()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Static(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource_Static(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCanyon::Ready_Components(const _tchar* pComponentTag)
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
	

	return S_OK;
}

HRESULT CCanyon::Ready_Col(const _tchar* pComponentTag)
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
	tUserData.szActorTag = TEXT("Canyon_Actor");

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

HRESULT CCanyon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

void CCanyon::SetCullingCollider(_uint iObjectID)
{
	auto pCullingCollider = static_cast<COBBCollider*>(m_pCullingCollider);
	switch (iObjectID)
	{
	case 1:
		pCullingCollider->SetCollision({ 0.f, 2.0f, 0.f }, {}, { 3, 3, 32 });
		break;
	case 2: case 9: case 10: case 13: case 14: case 16:
	case 30: case 62: case 63:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 3, 2, 2 });
		break;
	case 3:
		pCullingCollider->SetCollision({ 0.f, 8.67f, 0.f }, {}, { 13, 13, 3 });
		break;
	case 4: case 8: case 11: case 15: case 25:
	case 28: case 31: case 32: case 64:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 2, 2, 2 });
		break;
	case 5:
		pCullingCollider->SetCollision({ 0.f, 3.33f, 0.f }, {}, { 10, 5, 4 });
		break;
	case 6: case 44:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 4, 2, 1 });
		break;
	case 7:
		pCullingCollider->SetCollision({ 0.f, 43.33f, 0.f }, {}, { 18, 65, 18 });
		break;
	case 12:
		pCullingCollider->SetCollision({ 0.f, 2.0f, 0.f }, {}, { 4, 3, 4 });
		break;
	case 17:
		pCullingCollider->SetCollision({ 0.f, 4.67f, 0.f }, {}, { 15, 7, 7 });
		break;
	case 18:
		pCullingCollider->SetCollision({ 0.f, 9.33f, 0.f }, {}, { 11, 14, 11 });
		break;
	case 19:
		pCullingCollider->SetCollision({ 0.f, 8.67f, 0.f }, {}, { 13, 13, 5 });
		break;
	case 20:
		pCullingCollider->SetCollision({ 0.f, 3.33f, 0.f }, {}, { 14, 5, 4 });
		break;
	case 21:
		pCullingCollider->SetCollision({ 0.f, 2.67f, 0.f }, {}, { 18, 4, 8 });
		break;
	case 22:
		pCullingCollider->SetCollision({ 0.f, 23.33f, 0.f }, {}, { 12, 35, 9 });
		break;
	case 23:
		pCullingCollider->SetCollision({ 0.f, 16.67f, 0.f }, {}, { 22, 25, 10 });
		break;
	case 24:
		pCullingCollider->SetCollision({ 0.f, 15.33f, 0.f }, {}, { 35, 23, 10 });
		break;
	case 26: case 27:
		pCullingCollider->SetCollision({ 0.f, 0.67f, 0.f }, {}, { 4, 1, 4 });
		break;
	case 29:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 3, 2, 3 });
		break;
	case 33:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 4, 2, 3 });
		break;
	case 34:
		pCullingCollider->SetCollision({ 0.f, 2.67f, 0.f }, {}, { 5, 4, 5 });
		break;
	case 35: case 47:
		pCullingCollider->SetCollision({ 0.f, 13.33f, 0.f }, {}, { 55, 20, 30 });
		break;
	case 36:
		pCullingCollider->SetCollision({ 0.f, 12.0f, 0.f }, {}, { 62, 18, 30 });
		break;
	case 37:
		pCullingCollider->SetCollision({ 0.f, 20.67f, 0.f }, {}, { 58, 31, 15 });
		break;
	case 38:
		pCullingCollider->SetCollision({ 0.f, 12.0f, 0.f }, {}, { 15, 18, 8 });
		break;
	case 39:
		pCullingCollider->SetCollision({ 0.f, 22.67f, 0.f }, {}, { 50, 34, 40 });
		break;
	case 40:
		pCullingCollider->SetCollision({ 0.f, 5.33f, 0.f }, {}, { 15, 8, 10 });
		break;
	case 41:
		pCullingCollider->SetCollision({ 0.f, 26.67f, 0.f }, {}, { 25, 40, 40 });
		break;
	case 42:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 2, 2, 1 });
		break;
	case 43:
		pCullingCollider->SetCollision({ 0.f, 2.0f, 0.f }, {}, { 5, 3, 2 });
		break;
	case 45:
		pCullingCollider->SetCollision({ 0.f, 12.0f, 0.f }, {}, { 60, 18, 25 });
		break;
	case 46:
		pCullingCollider->SetCollision({ 0.f, 3.33f, 0.f }, {}, { 18, 5, 23 });
		break;
	case 48:
		pCullingCollider->SetCollision({ 0.f, 10.0f, 0.f }, {}, { 15, 15, 10 });
		break;
	case 49:
		pCullingCollider->SetCollision({ 0.f, 13.33f, 0.f }, {}, { 20, 20, 13 });
		break;
	case 50:
		pCullingCollider->SetCollision({ 0.f, 10.0f, 0.f }, {}, { 30, 15, 18 });
		break;
	case 51:
		pCullingCollider->SetCollision({ 0.f, 16.67f, 0.f }, {}, { 35, 25, 15 });
		break;
	case 52:
		pCullingCollider->SetCollision({ 0.f, 18.67f, 0.f }, {}, { 28, 28, 17 });
		break;
	case 53:
		pCullingCollider->SetCollision({ 0.f, 8.0f, 0.f }, {}, { 105, 12, 45 });
		break;
	case 54:
		pCullingCollider->SetCollision({ 0.f, 5.33f, 0.f }, {}, { 20, 8, 10 });
		break;
	case 55:
		pCullingCollider->SetCollision({ 0.f, 4.0f, 0.f }, {}, { 28, 6, 10 });
		break;
	case 56:
		pCullingCollider->SetCollision({ 0.f, 1.33f, 0.f }, {}, { 25, 2, 7 });
		break;
	case 57:
		pCullingCollider->SetCollision({ 0.f, 2.67f, 0.f }, {}, { 22, 4, 7 });
		break;
	case 58:
		pCullingCollider->SetCollision({ 0.f, 2.67f, 0.f }, {}, { 25, 4, 10 });
		break;
	case 59:
		pCullingCollider->SetCollision({ 0.f, 2.0f, 0.f }, {}, { 15, 3, 8 });
		break;
	case 60:
		pCullingCollider->SetCollision({ 0.f, 0.67f, 0.f }, {}, { 4, 1, 1 });
		break;
	case 61:
		pCullingCollider->SetCollision({ 0.f, 0.67f, 0.f }, {}, { 3, 1, 1 });
		break;
	case 65:
		pCullingCollider->SetCollision({ 0.f, 8.0f, 0.f }, {}, { 8, 12, 8 });
		break;
	case 66:
		pCullingCollider->SetCollision({ 0.f, 8.67f, 0.f }, {}, { 15, 13, 6 });
		break;
	case 67: case 68: case 69: case 70: case 71: case 72:
	case 73: case 74: case 76: case 77: case 78: case 79:
		pCullingCollider->SetCollision({ 0.f, 333.33f, 0.f }, {}, { 1000, 500, 1000 });
		break;
	case 75:
		pCullingCollider->SetCollision({ 0.f, 0.67f, 0.f }, {}, { 6, 1, 6 });
		break;
	}
}

CCanyon* CCanyon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanyon* pInstance = new CCanyon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCanyon::Clone(void* pArg)
{
	CCanyon* pInstance = new CCanyon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanyon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCanyon::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
