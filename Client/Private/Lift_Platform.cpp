#include "pch.h"
#include "Lift_Platform.h"
#include "GameInstance.h"

CLift_Platform::CLift_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CLift_Platform::CLift_Platform(const CLift_Platform& Prototype)
	: CActor{ Prototype }
{
}

HRESULT CLift_Platform::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLift_Platform::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ACTOR_DESC* pDesc = static_cast<ACTOR_DESC*>(pArg);
	if (FAILED(Ready_Components(pDesc->szVIBuffer_PrototypeName)))
		return E_FAIL;

	_matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	m_pCullingCollider->UpdateColiision(WorldMat);
	m_pRigidBody->Update_PxTransform(WorldMat);

	XMStoreFloat3(&m_vRootPos, m_pTransformCom->Get_State(STATE::POSITION));
	return S_OK;
}

void CLift_Platform::Priority_Update(_float fTimeDelta)
{
}

void CLift_Platform::Update(_float fTimeDelta)
{
	if (m_bIsPaltformMove)
	{
		LerpTargetPoint(fTimeDelta);

		_matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
		m_pCullingCollider->UpdateColiision(WorldMat);
		m_pRigidBody->Update_PxTransform(WorldMat);
	}
}

void CLift_Platform::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_DebugComponent(m_pCullingCollider);
	if (m_pGameInstance->isIn_WorldFrustum(m_pCullingCollider))
	{
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
		// 얜 움직이니까 업데이트 해줘야합니다
		m_pRigidBody->Update_PxTransform(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	}
}

HRESULT CLift_Platform::Render()
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

_bool CLift_Platform::SetPlatformMove(LIFT_PLATFORM_STATE eState)
{
	if (m_bIsPaltformMove)
		return false;

	_vector vPlatformPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vDir = {};

	switch (eState)
	{
	case CLift_Platform::LIFT_PLATFORM_STATE::UPPER:
	{
		vDir = m_pTransformCom->Get_State(STATE::UP);
		_vector vTargetPos = XMLoadFloat3(&m_vRootPos) + vDir * m_fMoveDistance;
		_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vPlatformPos));
		
		// 여기서 위인지 아래인지
		if (fDistance > 0.1f)
		{
			XMStoreFloat3(&m_vTargetPoint, vTargetPos);
			m_ePlatform_State = eState;
			m_bIsPaltformMove = true;
		}
		else
			XMStoreFloat3(&m_vTargetPoint, vPlatformPos);
	}
		break;
	case CLift_Platform::LIFT_PLATFORM_STATE::DWON:
	{
		_vector vTargetPos = XMLoadFloat3(&m_vRootPos);
		_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vPlatformPos));

		// 여기서 위인지 아래인지
		if (fDistance > 0.1f)
		{
			XMStoreFloat3(&m_vTargetPoint, vTargetPos);
			m_ePlatform_State = eState;
			m_bIsPaltformMove = true;
		}
		else
			XMStoreFloat3(&m_vTargetPoint, vPlatformPos);
	}
		break;
	}
	
	return false;
}

void CLift_Platform::LerpTargetPoint(_float fTimeDelta)
{
	_vector vPlatformPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vTargetPos = XMLoadFloat3(&m_vTargetPoint);

	_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vPlatformPos));
	if (fDistance < 0.1f)
		m_bIsPaltformMove = false;

	_vector vLerpPos = XMVectorLerp(vPlatformPos, vTargetPos, fTimeDelta * m_vLerpSpeed);
	m_pTransformCom->Set_State(STATE::POSITION, vLerpPos);
}

HRESULT CLift_Platform::Ready_Components(const _tchar* pComponentTag)
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), pComponentTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	m_pGameInstance->Add_RigidBody_ToPhysx(this, m_pRigidBody);
	static_cast<COBBCollider*>(m_pCullingCollider)->SetCollision({}, {}, { 2.f, 2.f, 2.f });

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
	tUserData.szActorTag = TEXT("Elavator_Actor");

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
	// ->> 엘레베이터는 몸체 제외하고 고정되어있으니까 STATIC으로 세팅 해주는거에요.
	// -> 얜 플랫폼이라 키네마틱으로 세팅해줘요.

	RigidBodyDesc.eRigidBodyType = CRigidBody::RIGIDBODY_TYPE::KINEMATIC;

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

HRESULT CLift_Platform::Bind_ShaderResources()
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

CLift_Platform* CLift_Platform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLift_Platform* pInstance = new CLift_Platform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLift_Platform::Clone(void* pArg)
{
	CLift_Platform* pInstance = new CLift_Platform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLift_Platform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLift_Platform::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
