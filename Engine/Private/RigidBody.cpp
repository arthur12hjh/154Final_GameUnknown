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
	: CComponent{ Prototype }
	, m_pPxPhysics{ m_pGameInstance->Get_PxPhysics() }
{
}

void CRigidBody::Set_Simulation(_bool bFlag)
{
	// ������ �ٵ��� �ùķ��̼� ��Ȱ��ȭ
	m_pPxRigidBody->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, bFlag);

	if (m_eShape != RIGIDBODY_SHAPE::NONE && m_eShape != RIGIDBODY_SHAPE::END)
	{
		_uint iNumShapes = m_pPxRigidBody->getNbShapes();
		vector<PxShape*> AllShapes(iNumShapes);

		m_pPxRigidBody->getShapes(AllShapes.data(), iNumShapes);

		for (auto& Shape : AllShapes)
		{
			Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, bFlag);
			Shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, bFlag);
		}

	}
}

void CRigidBody::Set_AngularDamping(_float fValue)
{
	static_cast<PxRigidDynamic*>(m_pPxRigidBody)->setAngularDamping(fValue);
}

void CRigidBody::Set_LinearDamping(_float fValue)
{
	static_cast<PxRigidDynamic*>(m_pPxRigidBody)->setLinearDamping(fValue);
}

void CRigidBody::Set_CCD(_bool bFlag)
{
	static_cast<PxRigidDynamic*>(m_pPxRigidBody)->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, bFlag);
}

//default 0.02f
void CRigidBody::Set_ContactOffset(_float fValue)
{
	Get_PxShape()->setContactOffset(fValue);
}

//default 0.f
void CRigidBody::Set_RestOffset(_float fValue)
{
	Get_PxShape()->setRestOffset(fValue);
}

void CRigidBody::Set_LocalPos(_float3 vLocalPos)
{
	PxShape* pShape = Get_PxShape();
	if (nullptr == pShape)
		return;

	// ���� ���� ȸ�� ����
	PxTransform localPose = pShape->getLocalPose();

	localPose.p = PxVec3(vLocalPos.x, vLocalPos.y, vLocalPos.z);

	pShape->setLocalPose(localPose);
}

HRESULT CRigidBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	RIGIDBODY_DESC* pDesc = static_cast<RIGIDBODY_DESC*>(pArg);
		
	m_isSimulateSync = pDesc->isSimulateSync;

	if (FAILED(Ready_PxMaterial(pDesc)))
		return E_FAIL;
	
	if (RIGIDBODY_SHAPE::NONE != pDesc->eRigidBodyShape)
	{
		if (FAILED(Ready_PxShape(pDesc)))
			return E_FAIL;
	}
	else
		m_eShape = RIGIDBODY_SHAPE::NONE;

	if (FAILED(Ready_PxRigidBody(pDesc)))
		return E_FAIL;

    return S_OK;
}

void CRigidBody::Update_PxTransform(_fmatrix vWorldMatrix, _bool isKinematicTarget)
{
	if (true == m_isFastArcEnabled)
	{
		PxRigidDynamic* pRigidDynamic = m_pPxRigidBody->is<PxRigidDynamic>();
		if (pRigidDynamic)
		{
			_float fGravityMul = m_fPlayRate * m_fPlayRate;
			PxVec3 vGravity = m_pGameInstance->Get_PxScene()->getGravity();
			PxVec3 vExtraForce = vGravity * (fGravityMul - 1.0f) * pRigidDynamic->getMass();

			pRigidDynamic->addForce(vExtraForce, PxForceMode::eFORCE, true);
		}
	}

	if (RIGIDBODY_TYPE::KINEMATIC == m_eType && true == isKinematicTarget)
	{
		auto pRigidDynamic = m_pPxRigidBody->is<PxRigidDynamic>();
		if(pRigidDynamic)
			pRigidDynamic->setKinematicTarget(PxTransform(m_pGameInstance->Convert_Matrix_ToPxTransform(vWorldMatrix)));
	}
	else
		m_pPxRigidBody->setGlobalPose(PxTransform(m_pGameInstance->Convert_Matrix_ToPxTransform(vWorldMatrix)));
}

void CRigidBody::Add_Impulse(_vector vImpulseDir, _float fPower, _float fPlayRate)
{
	if (RIGIDBODY_TYPE::DYNAMIC != m_eType)
		return;

	auto* pDyn = m_pPxRigidBody ? m_pPxRigidBody->is<PxRigidDynamic>() : nullptr;
	if (!pDyn)
		return;

	// ���� ����(Ȥ�� ���� 0 ������ ����)
	PxVec3 dir(XMVectorGetX(vImpulseDir), XMVectorGetY(vImpulseDir), XMVectorGetZ(vImpulseDir));
	if (dir.magnitudeSquared() < 1e-8f)
		return;

	dir.normalize();

	// 2��� ����: �ʱ� �ӵ��� k�� (impulse�� k��)
	PxVec3 vImpulse = dir * (fPower * fPlayRate);
	pDyn->addForce(vImpulse, PxForceMode::eIMPULSE, true);

	// (���� �� ���õ�) -> ��� �̷� �� ����/�ʱ�ȭ �� 1ȸ�� �̻���������,
	// ������ �ּ� ���� ����
	pDyn->setMaxLinearVelocity(100.f);
	pDyn->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	pDyn->setSolverIterationCounts(8, 4);
	pDyn->setSleepThreshold(0.3f);

	// FastArc(���� �߷�) Ȱ��ȭ
	m_isFastArcEnabled = (fPlayRate > 1.0f);
	m_fPlayRate = fPlayRate;
}

HRESULT CRigidBody::Ready_PxMaterial(RIGIDBODY_DESC* pDesc)
{
	/* ���׸��� ���� */
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
	/* �ڽ��� �׳� x, y ,z �� ���� ���� ��Ÿ��.. */
	case RIGIDBODY_SHAPE::BOX:
		m_pShape = m_pPxPhysics->createShape(PxBoxGeometry(m_vSize.x / 2.f, m_vSize.y / 2.f, m_vSize.z / 2.f), *m_pMaterial, true, ShapeFlags);
		m_pShape->setSimulationFilterData(Filter);
		m_pShape->setQueryFilterData(Filter);
		break;
	/* ĸ���� x �κ��� �� �κ� ������, y �κ��� ���� ����. */
	case RIGIDBODY_SHAPE::CAPSULE:
		m_pShape = m_pPxPhysics->createShape(PxCapsuleGeometry(m_vSize.x, m_vSize.y), *m_pMaterial, true, ShapeFlags);
		m_pShape->setSimulationFilterData(Filter);
		m_pShape->setQueryFilterData(Filter);
		break;

	/* ���� x�κи� ���������� ���. */
	case RIGIDBODY_SHAPE::SPHERE:
		m_pShape = m_pPxPhysics->createShape(PxSphereGeometry(m_vSize.x), *m_pMaterial, true, ShapeFlags);
		m_pShape->setSimulationFilterData(Filter);
		m_pShape->setQueryFilterData(Filter);
		break;

	/* �÷����� ũ�Ⱑ ���� ���Ѵ븸 ����. */
	case RIGIDBODY_SHAPE::PLANE:
		m_pShape = m_pPxPhysics->createShape(PxPlaneGeometry(), *m_pMaterial, true, ShapeFlags);
		m_pShape->setSimulationFilterData(Filter);
		m_pShape->setQueryFilterData(Filter);
		break;
	/* �浹�� �޽� ���� ���� */
	case RIGIDBODY_SHAPE::TRIANGLE:
	{
		if (pDesc->pColModel == nullptr)
			return E_FAIL;

		_uint iMeshNum = pDesc->pColModel->Get_NumMeshes();

		// TriangleMesh�� ���� ���� �� �����Ƿ� Shape ����Ʈ ����
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

			// TriangleMeshDesc ����
			PxTriangleMeshDesc meshDesc;
			meshDesc.points.count = iNumVertices;
			meshDesc.points.stride = sizeof(PxVec3);
			meshDesc.points.data = Vertices.data();

			meshDesc.triangles.count = iNumPrimitives;
			meshDesc.triangles.stride = sizeof(PxU32) * 3;
			meshDesc.triangles.data = Indices.data();

			// �Ｎ Cooking API
			PxTriangleMesh* pTriangleMesh =
				PxCreateTriangleMesh(PxCookingParams(PxTolerancesScale(1.0f)), meshDesc);

			if (pTriangleMesh == nullptr)
				continue;

			PxMeshScale vMeshScale(PxVec3(pDesc->vSize.x, pDesc->vSize.y, pDesc->vSize.z));
			PxTriangleMeshGeometry MeshGeometry(pTriangleMesh, vMeshScale);

			// Shape ����

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
	m_tUserData.pWord0 = &pDesc->iCollisionGroup;
	m_tUserData.pWord1 = &pDesc->iCollisionMask;

	//ĸ���ϋ� ȸ����������.
	if (RIGIDBODY_SHAPE::CAPSULE == m_eShape && nullptr != m_pShape)
	{
		PxTransform localPose(PxIdentity);
		localPose.q = PxQuat(PxPi * 0.5f, PxVec3(0.f, 0.f, 1.f));

		localPose.p = PxVec3(0.f, 0.f, 0.f);

		m_ShapeLocalPose = localPose;
		m_pShape->setLocalPose(localPose);
	}

	return S_OK;
}

HRESULT CRigidBody::Ready_PxRigidBody(RIGIDBODY_DESC* pDesc)
{
	/* ������ �ٵ� ���� */
	m_eType = pDesc->eRigidBodyType;
	/* ���� ���� */
	m_fMass = pDesc->fMass;

	PxTransform Transform = PxTransform(m_pGameInstance->Convert_Matrix_ToPxTransform(XMLoadFloat4x4(&pDesc->StartWorldMatrix)));

	switch (m_eType)
	{
	case RIGIDBODY_TYPE::DYNAMIC:
		m_pPxRigidBody = m_pPxPhysics->createRigidDynamic(Transform);
		break;

	case RIGIDBODY_TYPE::KINEMATIC:
		m_pPxRigidBody = m_pPxPhysics->createRigidDynamic(Transform);
		static_cast<PxRigidDynamic*>(m_pPxRigidBody)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		// Kinematic�� ����, ���� ������Ʈ ���� ����
		break;

	case RIGIDBODY_TYPE::STATIC:
		m_pPxRigidBody = m_pPxPhysics->createRigidStatic(Transform);
		// Static�� ����, ���� ����
		break;
	}

	//Shape �������� ������ ���⼭ ���߰� ������.
	if (RIGIDBODY_SHAPE::NONE == m_eShape)
		return S_OK;

	//Ʈ���̾ޱ��� ����ó�� ���ش�.
	if (RIGIDBODY_SHAPE::TRIANGLE == m_eShape)
	{
		for (auto& pShape : m_TriangleShapes)
		{
			m_pPxRigidBody->attachShape(*pShape);
		}
	}
	/* Shape ���̱� */
	else
	{
		m_pPxRigidBody->attachShape(*m_pShape);
		if(RIGIDBODY_TYPE::DYNAMIC == m_eType && true == pDesc->isActiveMass)
			physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<PxRigidDynamic*>(m_pPxRigidBody), m_fMass);
	}

	/* ���� ������ ���� */
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
