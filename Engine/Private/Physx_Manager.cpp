#include "Physx_Manager.h"

#include "GameObject.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "Physx_FilterShader.h"
#include "PxDefaultContactCallback.h"

CPhysx_Manager::CPhysx_Manager()
{
}

HRESULT CPhysx_Manager::Initialize()
{
    m_pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(m_pGameInstance);
    /* ������ �ʱ�ȭ */
    m_pSceneEventCallback = PxDefaultContactCallback::Create();

	m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocator, m_DefaultErrorCallback);

    m_Pvd = PxCreatePvd(*m_PxFoundation);
    
    m_PxTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_Pvd->connect(*m_PxTransport, PxPvdInstrumentationFlag::eALL);

    m_PxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), true, m_Pvd);
	if (nullptr == m_PxPhysics)
		return E_FAIL;

    PxInitExtensions(*m_PxPhysics, m_Pvd);

    PxSceneDesc sceneDesc(m_PxPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

    m_PxDispatcher = PxDefaultCpuDispatcherCreate(2);

    sceneDesc.cpuDispatcher = m_PxDispatcher;
    sceneDesc.filterShader = MyFilterShader;
	sceneDesc.simulationEventCallback = m_pSceneEventCallback;

    m_PxScene = m_PxPhysics->createScene(sceneDesc);

    m_pPxCCTManager = PxCreateControllerManager(*m_PxScene);

    PxPvdSceneClient* pvdClient = m_PxScene->getScenePvdClient();
   
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    return S_OK;
}

void CPhysx_Manager::Update(_float fTimeDelta)
{
    /* Dead Pair Remove */
    for (auto CCTPair = m_CCTs.begin(); CCTPair != m_CCTs.end();)
    {
        if (CCTPair->first->isDead())
        {
            Safe_Release(CCTPair->first);
            Safe_Release(CCTPair->second);

            CCTPair = m_CCTs.erase(CCTPair); // Erase Pair CCT
        }
        else
        {
            ++CCTPair;
        }
    }

    for (auto RigidBodyPair = m_RigidBodies.begin(); RigidBodyPair != m_RigidBodies.end();)
    {
        if (RigidBodyPair->first->isDead())
        {
            m_PxScene->removeActor(*RigidBodyPair->second->Get_PxRigidBody());
            Safe_Release(RigidBodyPair->first);
            Safe_Release(RigidBodyPair->second);

            RigidBodyPair = m_RigidBodies.erase(RigidBodyPair);
        }
        else
        {
            ++RigidBodyPair;
        }
    }

    //피직스 시뮬레이션 실행
    if (nullptr != m_PxScene)
    {
        m_PxScene->simulate(1 / 60.f);      // 고정상수 슛~
        m_PxScene->fetchResults(true);        // true로 해야 PVD에서 볼 수 있음.

        //트랜스폼 동기화 세팅
        for (auto& Pair : m_RigidBodies)
        {
            CTransform* pTransform = Pair.first->GetTransform();
            PxTransform pPxTransform = Pair.second->Get_PxTransform();

            if (true == Pair.second->isSimulateSync())
            {
                const PxVec3& vPxPosition = pPxTransform.p;
                pTransform->Set_State(STATE::POSITION, XMVectorSet(vPxPosition.x, vPxPosition.y, vPxPosition.z, 1.f));
            
                const PxQuat& vPxRotation = pPxTransform.q;
                pTransform->Rotation(vPxRotation.x, vPxRotation.y, vPxRotation.z, vPxRotation.w);

                pTransform->Update_PreWorldMatrix();
            }

#ifdef _DEBUG
            m_pGameInstance->Add_PhysxGeometry(Pair.first, Pair.second->Get_PxRigidBody(), Pair.second->Get_PxShape());
#endif
        }

        for (auto& Pair : m_CCTs)
        {
            Pair.second->Update_ControllerTransform(fTimeDelta, Pair.first->GetTransform());
#ifdef _DEBUG
            m_pGameInstance->Add_PhysxGeometry(Pair.first, Pair.second->Get_PxActor(), Pair.second->Get_PxShape());
#endif
        }
    }
}

void CPhysx_Manager::TestSetting()
{
    physx::PxMaterial* mMaterial = NULL;
    // create simulation
    mMaterial = m_PxPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    physx::PxRigidStatic* groundPlane = PxCreatePlane(*m_PxPhysics, physx::PxPlane(0, 1, 0, 0), *mMaterial);
    m_PxScene->addActor(*groundPlane);
}

void CPhysx_Manager::Clear()
{
    for (auto& RigidBodyPair : m_RigidBodies)
    {
        m_PxScene->removeActor(*RigidBodyPair.second->Get_PxRigidBody());
        Safe_Release(RigidBodyPair.first);
        Safe_Release(RigidBodyPair.second);
    }
    m_RigidBodies.clear();

    for (auto& CCTPair : m_CCTs)
    {
        Safe_Release(CCTPair.first);
        Safe_Release(CCTPair.second);
    }

    m_CCTs.clear();

    for (auto& Terrain : m_pTerrains)
    {
        m_PxScene->removeActor(*Terrain);
        Terrain->release();
    }

    m_pTerrains.clear();

    if (nullptr != m_pHeightField)
    {
        m_pHeightField->release();
        m_pHeightField = nullptr;
    }
}

HRESULT CPhysx_Manager::Add_CCT_ToPhysx(CGameObject* pGameObject, CCharacterController* pCCT)
{
    if (nullptr == pGameObject || nullptr == pCCT)
        return E_FAIL;

    m_CCTs.push_back(make_pair(pGameObject, pCCT));

    Safe_AddRef(pGameObject);
    Safe_AddRef(pCCT);

    return S_OK;
}

HRESULT CPhysx_Manager::Add_RigidBody_ToPhysx(CGameObject* pGameObject, CRigidBody* pRigidBody)
{
    if (nullptr == pGameObject || nullptr == pRigidBody)
        return E_FAIL;

    //�̰� SetOwner �Ǹ� ����
    //if (pRigidBody->GetOwner() != pGameObject)
    //    return E_FAIL;

    m_RigidBodies.push_back(make_pair(pGameObject, pRigidBody));

    m_PxScene->addActor(*pRigidBody->Get_PxRigidBody());

    Safe_AddRef(pGameObject);
    Safe_AddRef(pRigidBody);

    return S_OK;
}

HRESULT CPhysx_Manager::Add_Terrain_ToPhysx(CVIBuffer_Terrain* pTerrainVIBuffer)
{
    //���� ó��
    if (nullptr == pTerrainVIBuffer)
        return E_FAIL;

    if (nullptr != m_pHeightField)
    {
        m_pHeightField->release();
        m_pHeightField = nullptr;
    }

    _uint iNumVerticesX = pTerrainVIBuffer->Get_NumVerticesX();
    _uint iNumVerticesZ = pTerrainVIBuffer->Get_NumVerticesZ();
    _float* pHeightData = pTerrainVIBuffer->Get_HeightData();

    //�ͷ��� ���� ������ŭ ����.
    PxHeightFieldSample* pSamples = new PxHeightFieldSample[iNumVerticesX * iNumVerticesZ];

    //���ɸ԰� �ͷ��� �߰��ϴ� ���� �ۼ�

    for (_uint iRow = 0; iRow < iNumVerticesZ; ++iRow)
    {
        for (_uint iCol = 0; iCol < iNumVerticesX; ++iCol)
        {
            // Terrain heightData = height[z][x]
            // PhysX heightField  = sample[row][col]

            // X/Z swap: height[x][z]
            _uint srcIdx = iCol * iNumVerticesZ + iRow;           // ���ҵ� �ҽ� �ε���
            _uint dstIdx = iRow * iNumVerticesX + iCol;           // PhysX�� �䱸�ϴ� �ε���


            _float fHeight = pHeightData[srcIdx] * 100.f;

            pSamples[dstIdx].height = (PxI16)fHeight;
            pSamples[dstIdx].materialIndex0 = 0;
            pSamples[dstIdx].materialIndex1 = 0;
            pSamples[dstIdx].clearTessFlag();
        }
    }

    PxHeightFieldDesc HeightFieldDesc;

    HeightFieldDesc.nbColumns = iNumVerticesX;
    HeightFieldDesc.nbRows = iNumVerticesZ;
    HeightFieldDesc.samples.data = pSamples;
    HeightFieldDesc.samples.stride = sizeof(PxHeightFieldSample);

    PxHeightField* pHeightField = PxCreateHeightField(HeightFieldDesc);

    PxHeightFieldGeometry hfGeom(pHeightField, PxMeshGeometryFlags(), 1 / 100.0f, 1.0f, 1.0f);

    PxTransform pose(PxVec3(-0.5f, 0, -0.5f));

    PxMaterial* pMaterial = NULL;
    pMaterial = m_PxPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    PxShape* pTerrianShape = m_PxPhysics->createShape(hfGeom, *pMaterial);
    PxFilterData FilterData = {};
    FilterData.word0 = PHYSX_TERRAIN;
    FilterData.word1 = 0xFFFFFFFF;

    pTerrianShape->setQueryFilterData(FilterData);
    pTerrianShape->setSimulationFilterData(FilterData);

    PxRigidStatic* hfActor = m_PxPhysics->createRigidStatic(pose);
    hfActor->attachShape(*pTerrianShape);

    // �� ���
    m_PxScene->addActor(*hfActor);
    hfActor->setName("TERRAIN");
    
    m_pTerrains.push_back(hfActor);

    m_pHeightField = pHeightField;
    //pHeightField->release();
    Safe_Delete_Array(pHeightData);
    Safe_Delete_Array(pSamples);

    return S_OK;
}

PxTransform CPhysx_Manager::Convert_Matrix_ToPxTransform(_matrix WorldMatrix)
{
    _vector vTrans= WorldMatrix.r[3];

    _vector vRight = XMVector3Normalize(WorldMatrix.r[0]);
    _vector vUp = XMVector3Normalize(WorldMatrix.r[1]);

    vUp = XMVector3Normalize(vUp - vRight * XMVectorGetX(XMVector3Dot(vRight, vUp)));
    _vector vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));
    vUp = XMVector3Cross(vLook, vRight);

    _matrix Result = XMMatrixIdentity();
    Result.r[0] = vRight;
    Result.r[1] = vUp;
    Result.r[2] = vLook;

    _vector vRotation = XMQuaternionNormalize(XMQuaternionRotationMatrix(Result));

    PxVec3 p(XMVectorGetX(vTrans), XMVectorGetY(vTrans), XMVectorGetZ(vTrans));
    PxQuat pq(XMVectorGetX(vRotation), XMVectorGetY(vRotation), XMVectorGetZ(vRotation), XMVectorGetW(vRotation));

    return PxTransform(p, pq);
}

_matrix CPhysx_Manager::Convert_PxTransform_ToMatrix(PxTransform Transform)
{
    // ��ġ
    PxVec3  vPxPosition = Transform.p;
    _vector vPosition = XMVectorSet(vPxPosition.x, vPxPosition.y, vPxPosition.z, 1.0f);

    // ȸ�� (Quaternion)
    PxQuat  vPxQuaternion = Transform.q;
    _matrix vQuaternion = XMMatrixRotationQuaternion(XMVectorSet(vPxQuaternion.x, vPxQuaternion.y, vPxQuaternion.z, vPxQuaternion.w));

    // �������� 1��
    _vector vScale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

    // ��� ����
    return XMMatrixScalingFromVector(vScale) * vQuaternion * XMMatrixTranslationFromVector(vPosition);
}

CPhysx_Manager* CPhysx_Manager::Create()
{
    CPhysx_Manager* pInstance = new CPhysx_Manager();
    if (FAILED(pInstance->Initialize()))
    {
		MSG_BOX("Create Failed : PhysxManager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPhysx_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);

    PxCloseExtensions();
    
    for (auto& RigidBodyPair : m_RigidBodies)
    {
        m_PxScene->removeActor(*RigidBodyPair.second->Get_PxRigidBody());
        Safe_Release(RigidBodyPair.first);
        Safe_Release(RigidBodyPair.second);
    }
    m_RigidBodies.clear();

    for (auto& CCTPair : m_CCTs)
    {
        Safe_Release(CCTPair.first);
        Safe_Release(CCTPair.second);
    }

    m_CCTs.clear();
    
    if (nullptr != m_pHeightField)
    {
        m_pHeightField->release();
        m_pHeightField = nullptr;
    }

    for (auto& Terrain : m_pTerrains)
    {
        m_PxScene->removeActor(*Terrain);
        Terrain->release();
    }

    m_pTerrains.clear();

    if (nullptr != m_pPxCCTManager)
        m_pPxCCTManager->release();

    if (nullptr != m_PxScene)      
        m_PxScene->release();

    if (nullptr != m_PxDispatcher)
        m_PxDispatcher->release();

    if (nullptr != m_PxPhysics)
        m_PxPhysics->release();

    if (nullptr != m_Pvd)
        m_Pvd->release();

    if (nullptr != m_PxTransport)
        m_PxTransport->release();

    if (nullptr != m_PxFoundation)
        m_PxFoundation->release();

    Safe_Release(m_pSceneEventCallback);
}