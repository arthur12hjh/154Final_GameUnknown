#include "RigidBody.h"

#include "GameInstance.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
	, m_pPxPhysics { m_pGameInstance->Get_PxPhysics() }
{
}

CRigidBody::CRigidBody(const CRigidBody& Prototype)
    : CComponent { Prototype }
	, m_pPxPhysics{ m_pGameInstance->Get_PxPhysics() }
{
}

HRESULT CRigidBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	RIGIDBODY_DESC* pDesc = static_cast<RIGIDBODY_DESC*>(pArg);

	if (FAILED(Ready_PxTransform(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_PxMaterial(pDesc)))
		return E_FAIL;
	
	if (FAILED(Ready_PxShape(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_PxRigidBody(pDesc)))
		return E_FAIL;

    return S_OK;
}

void CRigidBody::Update_PxTransform(_fmatrix vWorldMatrix)
{
	/* 월드 위치 받아와서 트랜스폼 갱신 */
	/* 필요하면 추후 멤카피로 바꾸던가 해야됨.. */
	*m_pPxTransform = PxTransform(m_pGameInstance->Convert_Matrix_ToPxTransform(vWorldMatrix));
}

HRESULT CRigidBody::Ready_PxTransform(RIGIDBODY_DESC* pDesc)
{
	/* 트랜스폼 생성 */
	m_pPxTransform = new PxTransform(m_pGameInstance->Convert_Matrix_ToPxTransform(XMLoadFloat4x4(&pDesc->StartWorldMatrix)));

	return S_OK;
}

HRESULT CRigidBody::Ready_PxMaterial(RIGIDBODY_DESC* pDesc)
{
	/* 머테리얼 생성 */
	m_pMaterial = m_pPxPhysics->createMaterial(pDesc->vMaterial.x, pDesc->vMaterial.y, pDesc->vMaterial.z);

	return S_OK;
}

HRESULT CRigidBody::Ready_PxShape(RIGIDBODY_DESC* pDesc)
{
	PxTransform PrePxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));

	m_eShape = pDesc->eRigidBodyShape;
	m_vSize = pDesc->vSize;
	switch (m_eShape)
	{
	/* 박스는 그냥 x, y ,z 가 각각 지름 나타냄.. */
	case RIGIDBODY_SHAPE::BOX:
		m_pShape = m_pPxPhysics->createShape(PxBoxGeometry(m_vSize.x / 2.f, m_vSize.y / 2.f, m_vSize.z / 2.f), *m_pMaterial);
		break;
	/* 캡슐은 x 부분이 구 부분 반지름, y 부분이 높이 절반. */
	case RIGIDBODY_SHAPE::CAPSULE:
		m_pShape = m_pPxPhysics->createShape(PxCapsuleGeometry(m_vSize.x, m_vSize.y), *m_pMaterial);
		m_pShape->setLocalPose(PrePxTransform);
		break;

	/* 구는 x부분만 반지름으로 사용. */
	case RIGIDBODY_SHAPE::SPHERE:
		m_pShape = m_pPxPhysics->createShape(PxSphereGeometry(m_vSize.x), *m_pMaterial);
		break;

	/* 플레인은 크기가 없는 무한대만 생성. */
	case RIGIDBODY_SHAPE::PLANE:
		m_pShape = m_pPxPhysics->createShape(PxPlaneGeometry(), *m_pMaterial);
		break;

	//로직 많이 달라서 일단 E_FAIL 처리함.
	case RIGIDBODY_SHAPE::CONVEX:
		return E_FAIL;

	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRigidBody::Ready_PxRigidBody(RIGIDBODY_DESC* pDesc)
{
	/* 리지드 바디 생성 */
	m_eType = pDesc->eRigidBodyType;

	switch (m_eType)
	{
	case RIGIDBODY_TYPE::DYNAMIC:
		m_pPxRigidBody = m_pPxPhysics->createRigidDynamic(*m_pPxTransform);
		break;

	case RIGIDBODY_TYPE::KINEMATIC:
		m_pPxRigidBody = m_pPxPhysics->createRigidDynamic(*m_pPxTransform);
		static_cast<PxRigidDynamic*>(m_pPxRigidBody)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		break;

	case RIGIDBODY_TYPE::STATIC:
		m_pPxRigidBody = m_pPxPhysics->createRigidStatic(*m_pPxTransform);
		break;

	default:
		return E_FAIL;
	}
	/* Shape 붙이기 */
	m_pPxRigidBody->attachShape(*m_pShape);

	return S_OK;
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidBody* pInstance = new CRigidBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRigidBody");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRigidBody::Clone(void* pArg)
{
	CRigidBody* pInstance = new CRigidBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRigidBody");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRigidBody::Free()
{
	__super::Free();

	if (nullptr != m_pMaterial)
		m_pMaterial->release();

	if (nullptr != m_pShape)
		m_pShape->release();

	if (nullptr != m_pPxRigidBody)
		m_pPxRigidBody->release();

	if (nullptr != m_pPxTransform)
	{
		Safe_Delete(m_pPxTransform);
		m_pPxTransform = nullptr;
	}

	if (nullptr != m_pPxPhysics)
		m_pPxPhysics = nullptr;
}
