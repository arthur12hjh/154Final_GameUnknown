#include "RigidBody.h"

#include "GameInstance.h"
#include "Mesh.h"
#include "GameObject.h"

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
	if (true == m_isSyncByPhysx)
	{
		PxVec3 vExtraGravity(0.f, -9.8f * 8.0f, 0.f); // 2배 중력

		static_cast<PxRigidDynamic*>(m_pPxRigidBody)->addForce(vExtraGravity, PxForceMode::eACCELERATION);
	}
	else
	{
		if (RIGIDBODY_TYPE::KINEMATIC == m_eType)
			static_cast<PxRigidDynamic*>(m_pPxRigidBody)->setKinematicTarget(PxTransform(m_pGameInstance->Convert_Matrix_ToPxTransform(vWorldMatrix)));
		else
			m_pPxRigidBody->setGlobalPose(PxTransform(m_pGameInstance->Convert_Matrix_ToPxTransform(vWorldMatrix)));
	}
}

void CRigidBody::Add_Impulse(_vector vImpulseDir, _float fPower)
{
	if (RIGIDBODY_TYPE::DYNAMIC != m_eType)
		return;

	PxVec3 vDir = PxVec3(XMVectorGetX(vImpulseDir), XMVectorGetY(vImpulseDir), XMVectorGetZ(vImpulseDir)) * fPower;
	static_cast<PxRigidDynamic*>(m_pPxRigidBody)->addForce(vDir, PxForceMode::eIMPULSE);
	static_cast<PxRigidDynamic*>(m_pPxRigidBody)->setMaxLinearVelocity(100.f);
	static_cast<PxRigidDynamic*>(m_pPxRigidBody)->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	static_cast<PxRigidDynamic*>(m_pPxRigidBody)->setSolverIterationCounts(8, 4);
	static_cast<PxRigidDynamic*>(m_pPxRigidBody)->setSleepThreshold(0.1f);

	m_isSyncByPhysx = true;
}

HRESULT CRigidBody::Ready_PxMaterial(RIGIDBODY_DESC* pDesc)
{
	/* 머테리얼 생성 */
	m_pMaterial = m_pPxPhysics->createMaterial(pDesc->vMaterial.x, pDesc->vMaterial.y, pDesc->vMaterial.z);

	return S_OK;
}

HRESULT CRigidBody::Ready_PxShape(RIGIDBODY_DESC* pDesc)
{
	m_eShape = pDesc->eRigidBodyShape;
	m_vSize = pDesc->vSize;


	PxShapeFlags ShapeFlags = PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;

	if (false == pDesc->isQuery)
		ShapeFlags = PxShapeFlag::eSIMULATION_SHAPE;


	PxFilterData Filter;

	Filter.word0 = pDesc->iCollisionGroup;
	Filter.word1 = pDesc->iCollisionMask;

	switch (m_eShape)
	{
	/* 박스는 그냥 x, y ,z 가 각각 지름 나타냄.. */
	case RIGIDBODY_SHAPE::BOX:
		m_pShape = m_pPxPhysics->createShape(PxBoxGeometry(m_vSize.x / 2.f, m_vSize.y / 2.f, m_vSize.z / 2.f), *m_pMaterial, true, ShapeFlags);
		m_pShape->setSimulationFilterData(Filter);
		m_pShape->setQueryFilterData(Filter);
		break;
	/* 캡슐은 x 부분이 구 부분 반지름, y 부분이 높이 절반. */
	case RIGIDBODY_SHAPE::CAPSULE:
		m_pShape = m_pPxPhysics->createShape(PxCapsuleGeometry(m_vSize.x, m_vSize.y), *m_pMaterial, true, ShapeFlags);
		m_pShape->setSimulationFilterData(Filter);
		m_pShape->setQueryFilterData(Filter);
		break;

	/* 구는 x부분만 반지름으로 사용. */
	case RIGIDBODY_SHAPE::SPHERE:
		m_pShape = m_pPxPhysics->createShape(PxSphereGeometry(m_vSize.x), *m_pMaterial, true, ShapeFlags);
		m_pShape->setSimulationFilterData(Filter);
		m_pShape->setQueryFilterData(Filter);
		break;

	/* 플레인은 크기가 없는 무한대만 생성. */
	case RIGIDBODY_SHAPE::PLANE:
		m_pShape = m_pPxPhysics->createShape(PxPlaneGeometry(), *m_pMaterial, true, ShapeFlags);
		m_pShape->setSimulationFilterData(Filter);
		m_pShape->setQueryFilterData(Filter);
		break;
	/* 충돌용 메시 전용 로직 */
	case RIGIDBODY_SHAPE::TRIANGLE:
	{
		if (pDesc->pColModel == nullptr)
			return E_FAIL;

		_uint iMeshNum = pDesc->pColModel->Get_NumMeshes();

		// TriangleMesh는 여러 개일 수 있으므로 Shape 리스트 유지
		// (CRigidBody::m_pShape 를 첫 번째 shape 로 사용)
		vector<PxShape*> TriangleShapes;
		TriangleShapes.reserve(iMeshNum);

		for (_uint i = 0; i < iMeshNum; ++i)
		{
			CMesh* pMesh = pDesc->pColModel->Get_Mesh(i);

			const _float3* pVertexPositions = pMesh->Get_VertexPositionList();
			const _uint*   pIndices = pMesh->Get_Indices();
			_uint    iNumVertices = pMesh->Get_NumVectices();
			_uint    iNumIndices = pMesh->Get_NumIndices();
			_uint    iNumPrimitives = iNumIndices / 3;

			if (iNumVertices == 0 || iNumIndices == 0)
				continue;

			vector<PxVec3> Vertices;
			Vertices.reserve(iNumVertices);


			for (_uint v = 0; v < iNumVertices; ++v)
			{
				_vector vLocal = XMLoadFloat3(&pVertexPositions[v]);

				Vertices.push_back(PxVec3(
					XMVectorGetX(vLocal),
					XMVectorGetY(vLocal),
					XMVectorGetZ(vLocal)));
			}

			vector<PxU32> Indices;
			Indices.reserve(iNumIndices);
			for (_uint idx = 0; idx < iNumIndices; ++idx)
				Indices.push_back(pIndices[idx]);

			// TriangleMeshDesc 생성
			PxTriangleMeshDesc meshDesc;
			meshDesc.points.count = iNumVertices;
			meshDesc.points.stride = sizeof(PxVec3);
			meshDesc.points.data = Vertices.data();

			meshDesc.triangles.count = iNumPrimitives;
			meshDesc.triangles.stride = sizeof(PxU32) * 3;
			meshDesc.triangles.data = Indices.data();

			// 즉석 Cooking API
			PxTriangleMesh* pTriangleMesh =
				PxCreateTriangleMesh(PxCookingParams(PxTolerancesScale(1.0f)), meshDesc);

			if (pTriangleMesh == nullptr)
				continue;

			PxMeshScale vMeshScale(PxVec3(pDesc->vSize.x, pDesc->vSize.y, pDesc->vSize.z));
			PxTriangleMeshGeometry MeshGeometry(pTriangleMesh, vMeshScale);

			// Shape 생성

			PxShape* pShape = m_pPxPhysics->createShape(MeshGeometry, *m_pMaterial, ShapeFlags);

			pShape->setSimulationFilterData(Filter);
			pShape->setQueryFilterData(Filter);

			if (nullptr != pShape)
				m_TriangleShapes.push_back(pShape);

			Vertices.clear();
			Indices.clear();
			pTriangleMesh->release();
		}

		TriangleShapes.clear();

		break;
	}
	default:
		return E_FAIL;
	}

 	m_tUserData = pDesc->tUserData;

	return S_OK;
}

HRESULT CRigidBody::Ready_PxRigidBody(RIGIDBODY_DESC* pDesc)
{
	/* 리지드 바디 생성 */
	m_eType = pDesc->eRigidBodyType;
	/* 질량 세팅 */
	m_fMass = pDesc->fMass;

	PxTransform Transform = PxTransform(m_pGameInstance->Convert_Matrix_ToPxTransform(XMLoadFloat4x4(&pDesc->StartWorldMatrix)));

	switch (m_eType)
	{
	case RIGIDBODY_TYPE::DYNAMIC:
		m_pPxRigidBody = m_pPxPhysics->createRigidDynamic(Transform);
		physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<PxRigidDynamic*>(m_pPxRigidBody), m_fMass);
		break;

	case RIGIDBODY_TYPE::KINEMATIC:
		m_pPxRigidBody = m_pPxPhysics->createRigidDynamic(Transform);
		static_cast<PxRigidDynamic*>(m_pPxRigidBody)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		// Kinematic은 질량, 관성 업데이트 하지 않음
		break;

	case RIGIDBODY_TYPE::STATIC:
		m_pPxRigidBody = m_pPxPhysics->createRigidStatic(Transform);
		// Static은 질량, 관성 없음
		break;
	}

	//트라이앵글은 예외처리 해준다.
	if (m_eShape == RIGIDBODY_SHAPE::TRIANGLE)
	{
		for (auto& pShape : m_TriangleShapes)
		{
			m_pPxRigidBody->attachShape(*pShape);
		}
	}
	/* Shape 붙이기 */
	else
	{
		m_pPxRigidBody->attachShape(*m_pShape);
	}

	/* 유저 데이터 세팅 */
	m_tUserData.pHitActor = this;
	m_pPxRigidBody->userData = &m_tUserData;

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

	if (nullptr != m_pPxPhysics)
		m_pPxPhysics = nullptr;

	for (auto& pMesh : m_TriangleShapes)
		pMesh->release();
	m_TriangleShapes.clear();
}
