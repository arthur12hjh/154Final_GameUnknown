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
	if (!m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	{
		SetVisibility(VISIBILITY::HIDDEN);
		return;
	}

    /*if (GetVisibility() == VISIBILITY::VISIBLE)
    {
        m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
    }*/

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDER::OCCLUSION, this);
    

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
        // Size: {3.6, 3.6, 38.4}, Center Y: 3.6 * 0.75 = 2.7
        pCullingCollider->SetCollision({ 0.f, 2.7f, 0.f }, {}, { 3.6f, 3.6f, 38.4f });
        break;
    case 2: case 9: case 10: case 13: case 14: case 16:
    case 30: case 62: case 63:
        // Size: {3.6, 2.4, 2.4}, Center Y: 2.4 * 0.75 = 1.8
        pCullingCollider->SetCollision({ 0.f, 1.8f, 0.f }, {}, { 3.6f, 2.4f, 2.4f });
        break;
    case 3:
        // Size: {15.6, 15.6, 3.6}, Center Y: 15.6 * 0.75 = 11.7
        pCullingCollider->SetCollision({ 0.f, 11.7f, 0.f }, {}, { 15.6f, 15.6f, 3.6f });
        break;
    case 4: case 8: case 11: case 15: case 25:
    case 28: case 31: case 32: case 64:
        // Size: {2.4, 2.4, 2.4}, Center Y: 2.4 * 0.75 = 1.8
        pCullingCollider->SetCollision({ 0.f, 1.8f, 0.f }, {}, { 2.4f, 2.4f, 2.4f });
        break;
    case 5:
        // Size: {12.0, 6.0, 4.8}, Center Y: 6.0 * 0.75 = 4.5
        pCullingCollider->SetCollision({ 0.f, 4.5f, 0.f }, {}, { 12.f, 6.f, 4.8f });
        break;
    case 6: case 44:
        // Size: {4.8, 2.4, 1.2}, Center Y: 2.4 * 0.75 = 1.8
        pCullingCollider->SetCollision({ 0.f, 1.8f, 0.f }, {}, { 4.8f, 2.4f, 1.2f });
        break;
    case 7:
        // Size: {21.6, 78.0, 21.6}, Center Y: 78.0 * 0.75 = 58.5
        pCullingCollider->SetCollision({ 0.f, 58.5f, 0.f }, {}, { 21.6f, 78.f, 21.6f });
        break;
    case 12:
        // Size: {4.8, 3.6, 4.8}, Center Y: 3.6 * 0.75 = 2.7
        pCullingCollider->SetCollision({ 0.f, 2.7f, 0.f }, {}, { 4.8f, 3.6f, 4.8f });
        break;
    case 17:
        // Size: {18.0, 8.4, 8.4}, Center Y: 8.4 * 0.75 = 6.3
        pCullingCollider->SetCollision({ 0.f, 6.3f, 0.f }, {}, { 18.f, 8.4f, 8.4f });
        break;
    case 18:
        // Size: {13.2, 16.8, 13.2}, Center Y: 16.8 * 0.75 = 12.6
        pCullingCollider->SetCollision({ 0.f, 12.6f, 0.f }, {}, { 13.2f, 16.8f, 13.2f });
        break;
    case 19:
        // Size: {15.6, 15.6, 6.0}, Center Y: 15.6 * 0.75 = 11.7
        pCullingCollider->SetCollision({ 0.f, 11.7f, 0.f }, {}, { 15.6f, 15.6f, 6.f });
        break;
    case 20:
        // Size: {16.8, 6.0, 4.8}, Center Y: 6.0 * 0.75 = 4.5
        pCullingCollider->SetCollision({ 0.f, 4.5f, 0.f }, {}, { 16.8f, 6.f, 4.8f });
        break;
    case 21:
        // Size: {21.6, 4.8, 9.6}, Center Y: 4.8 * 0.75 = 3.6
        pCullingCollider->SetCollision({ 0.f, 3.6f, 0.f }, {}, { 21.6f, 4.8f, 9.6f });
        break;
    case 22:
        // Size: {14.4, 42.0, 10.8}, Center Y: 42.0 * 0.75 = 31.5
        pCullingCollider->SetCollision({ 0.f, 31.5f, 0.f }, {}, { 14.4f, 42.f, 10.8f });
        break;
    case 23:
        // Size: {26.4, 30.0, 12.0}, Center Y: 30.0 * 0.75 = 22.5
        pCullingCollider->SetCollision({ 0.f, 22.5f, 0.f }, {}, { 26.4f, 30.f, 12.f });
        break;
    case 24:
        // Size: {42.0, 27.6, 12.0}, Center Y: 27.6 * 0.75 = 20.7
        pCullingCollider->SetCollision({ 0.f, 20.7f, 0.f }, {}, { 42.f, 27.6f, 12.f });
        break;
    case 26: case 27:
        // Size: {4.8, 1.2, 4.8}, Center Y: 1.2 * 0.75 = 0.9
        pCullingCollider->SetCollision({ 0.f, 0.9f, 0.f }, {}, { 4.8f, 1.2f, 4.8f });
        break;
    case 29:
        // Size: {3.6, 2.4, 3.6}, Center Y: 2.4 * 0.75 = 1.8
        pCullingCollider->SetCollision({ 0.f, 1.8f, 0.f }, {}, { 3.6f, 2.4f, 3.6f });
        break;
    case 33:
        // Size: {4.8, 2.4, 3.6}, Center Y: 2.4 * 0.75 = 1.8
        pCullingCollider->SetCollision({ 0.f, 1.8f, 0.f }, {}, { 4.8f, 2.4f, 3.6f });
        break;
    case 34:
        // Size: {6.0, 4.8, 6.0}, Center Y: 4.8 * 0.75 = 3.6
        pCullingCollider->SetCollision({ 0.f, 3.6f, 0.f }, {}, { 6.f, 4.8f, 6.f });
        break;
    case 35: case 47:
        // Size: {66.0, 24.0, 36.0}, Center Y: 24.0 * 0.75 = 18.0
        pCullingCollider->SetCollision({ 0.f, 18.f, 0.f }, {}, { 66.f, 24.f, 36.f });
        break;
    case 36:
        // Size: {74.4, 21.6, 36.0}, Center Y: 21.6 * 0.75 = 16.2
        pCullingCollider->SetCollision({ 0.f, 16.2f, 0.f }, {}, { 74.4f, 21.6f, 36.f });
        break;
    case 37:
        // Size: {69.6, 37.2, 18.0}, Center Y: 37.2 * 0.75 = 27.9
        pCullingCollider->SetCollision({ 0.f, 27.9f, 0.f }, {}, { 69.6f, 37.2f, 18.f });
        break;
    case 38:
        // Size: {18.0, 21.6, 9.6}, Center Y: 21.6 * 0.75 = 16.2
        pCullingCollider->SetCollision({ 0.f, 16.2f, 0.f }, {}, { 18.f, 21.6f, 9.6f });
        break;
    case 39:
        // Size: {60.0, 40.8, 48.0}, Center Y: 40.8 * 0.75 = 30.6
        pCullingCollider->SetCollision({ 0.f, 30.6f, 0.f }, {}, { 60.f, 40.8f, 48.f });
        break;
    case 40:
        // Size: {18.0, 9.6, 12.0}, Center Y: 9.6 * 0.75 = 7.2
        pCullingCollider->SetCollision({ 0.f, 7.2f, 0.f }, {}, { 18.f, 9.6f, 12.f });
        break;
    case 41:
        // Size: {30.0, 48.0, 48.0}, Center Y: 48.0 * 0.75 = 36.0
        pCullingCollider->SetCollision({ 0.f, 36.f, 0.f }, {}, { 30.f, 48.f, 48.f });
        break;
    case 42:
        // Size: {2.4, 2.4, 1.2}, Center Y: 2.4 * 0.75 = 1.8
        pCullingCollider->SetCollision({ 0.f, 1.8f, 0.f }, {}, { 2.4f, 2.4f, 1.2f });
        break;
    case 43:
        // Size: {6.0, 3.6, 2.4}, Center Y: 3.6 * 0.75 = 2.7
        pCullingCollider->SetCollision({ 0.f, 2.7f, 0.f }, {}, { 6.f, 3.6f, 2.4f });
        break;
    case 45:
        // Size: {72.0, 21.6, 30.0}, Center Y: 21.6 * 0.75 = 16.2
        pCullingCollider->SetCollision({ 0.f, 16.2f, 0.f }, {}, { 72.f, 21.6f, 30.f });
        break;
    case 46:
        // Size: {21.6, 6.0, 27.6}, Center Y: 6.0 * 0.75 = 4.5
        pCullingCollider->SetCollision({ 0.f, 4.5f, 0.f }, {}, { 21.6f, 6.f, 27.6f });
        break;
    case 48:
        // Size: {18.0, 18.0, 12.0}, Center Y: 18.0 * 0.75 = 13.5
        pCullingCollider->SetCollision({ 0.f, 13.5f, 0.f }, {}, { 18.f, 18.f, 12.f });
        break;
    case 49:
        // Size: {24.0, 24.0, 15.6}, Center Y: 24.0 * 0.75 = 18.0
        pCullingCollider->SetCollision({ 0.f, 18.f, 0.f }, {}, { 24.f, 24.f, 15.6f });
        break;
    case 50:
        // Size: {36.0, 18.0, 21.6}, Center Y: 18.0 * 0.75 = 13.5
        pCullingCollider->SetCollision({ 0.f, 13.5f, 0.f }, {}, { 36.f, 18.f, 21.6f });
        break;
    case 51:
        // Size: {42.0, 30.0, 18.0}, Center Y: 30.0 * 0.75 = 22.5
        pCullingCollider->SetCollision({ 0.f, 22.5f, 0.f }, {}, { 42.f, 30.f, 18.f });
        break;
    case 52:
        // Size: {33.6, 33.6, 20.4}, Center Y: 33.6 * 0.75 = 25.2
        pCullingCollider->SetCollision({ 0.f, 25.2f, 0.f }, {}, { 33.6f, 33.6f, 20.4f });
        break;
    case 53:
        // Size: {126.0, 14.4, 54.0}, Center Y: 14.4 * 0.75 = 10.8
        pCullingCollider->SetCollision({ 0.f, 10.8f, 0.f }, {}, { 126.f, 14.4f, 54.f });
        break;
    case 54:
        // Size: {24.0, 9.6, 12.0}, Center Y: 9.6 * 0.75 = 7.2
        pCullingCollider->SetCollision({ 0.f, 7.2f, 0.f }, {}, { 24.f, 9.6f, 12.f });
        break;
    case 55:
        // Size: {33.6, 7.2, 12.0}, Center Y: 7.2 * 0.75 = 5.4
        pCullingCollider->SetCollision({ 0.f, 5.4f, 0.f }, {}, { 33.6f, 7.2f, 12.f });
        break;
    case 56:
        // Size: {30.0, 2.4, 8.4}, Center Y: 2.4 * 0.75 = 1.8
        pCullingCollider->SetCollision({ 0.f, 1.8f, 0.f }, {}, { 30.f, 2.4f, 8.4f });
        break;
    case 57:
        // Size: {26.4, 4.8, 8.4}, Center Y: 4.8 * 0.75 = 3.6
        pCullingCollider->SetCollision({ 0.f, 3.6f, 0.f }, {}, { 26.4f, 4.8f, 8.4f });
        break;
    case 58:
        // Size: {30.0, 4.8, 12.0}, Center Y: 4.8 * 0.75 = 3.6
        pCullingCollider->SetCollision({ 0.f, 3.6f, 0.f }, {}, { 30.f, 4.8f, 12.f });
        break;
    case 59:
        // Size: {18.0, 3.6, 9.6}, Center Y: 3.6 * 0.75 = 2.7
        pCullingCollider->SetCollision({ 0.f, 2.7f, 0.f }, {}, { 18.f, 3.6f, 9.6f });
        break;
    case 60:
        // Size: {4.8, 1.2, 1.2}, Center Y: 1.2 * 0.75 = 0.9
        pCullingCollider->SetCollision({ 0.f, 0.9f, 0.f }, {}, { 4.8f, 1.2f, 1.2f });
        break;
    case 61:
        // Size: {3.6, 1.2, 1.2}, Center Y: 1.2 * 0.75 = 0.9
        pCullingCollider->SetCollision({ 0.f, 0.9f, 0.f }, {}, { 3.6f, 1.2f, 1.2f });
        break;
    case 65:
        // Size: {9.6, 14.4, 9.6}, Center Y: 14.4 * 0.75 = 10.8
        pCullingCollider->SetCollision({ 0.f, 10.8f, 0.f }, {}, { 9.6f, 14.4f, 9.6f });
        break;
    case 66:
        // Size: {18.0, 15.6, 7.2}, Center Y: 15.6 * 0.75 = 11.7
        pCullingCollider->SetCollision({ 0.f, 11.7f, 0.f }, {}, { 18.f, 15.6f, 7.2f });
        break;
	case 67: case 68: case 69: case 70: case 71: case 72:
	case 73: case 74: case 76: case 77: case 78: case 79:
		pCullingCollider->SetCollision({ 0.f, 333.33f, 0.f }, {}, { 500, 500, 500 });
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
