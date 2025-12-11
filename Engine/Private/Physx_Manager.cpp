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
    /* 피직스 초기화 */
    m_pSceneEventCallback = PxDefaultContactCallback::Create();

	m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocator, m_DefaultErrorCallback);

    m_Pvd = physx::PxCreatePvd(*m_PxFoundation);
    
    m_PxTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_Pvd->connect(*m_PxTransport, physx::PxPvdInstrumentationFlag::eALL);

    m_PxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, physx::PxTolerancesScale(), true, m_Pvd);
	if (nullptr == m_PxPhysics)
		return E_FAIL;

    PxInitExtensions(*m_PxPhysics, m_Pvd);

    physx::PxSceneDesc sceneDesc(m_PxPhysics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

    m_PxDispatcher = physx::PxDefaultCpuDispatcherCreate(4);

    sceneDesc.cpuDispatcher = m_PxDispatcher;
    sceneDesc.filterShader = MyFilterShader;
	sceneDesc.simulationEventCallback = m_pSceneEventCallback;

    m_PxScene = m_PxPhysics->createScene(sceneDesc);

    m_pPxCCTManager = PxCreateControllerManager(*m_PxScene);

    physx::PxPvdSceneClient* pvdClient = m_PxScene->getScenePvdClient();
   
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    return S_OK;
}

void CPhysx_Manager::Update(_float fTimeDelta)
{
    /* Dead 오브젝트 제거 */
    for (auto CCTPair = m_CCTs.begin(); CCTPair != m_CCTs.end();)
    {
        if (CCTPair->first->isDead())
        {
            Safe_Release(CCTPair->first);
            Safe_Release(CCTPair->second);

            CCTPair = m_CCTs.erase(CCTPair); // erase는 다음 iterator 반환
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
            // 씬 내에서 강체 우선 제거 
            m_PxScene->removeActor(*RigidBodyPair->second->Get_PxRigidBody());
            Safe_Release(RigidBodyPair->first);
            Safe_Release(RigidBodyPair->second);

            RigidBodyPair = m_RigidBodies.erase(RigidBodyPair); // erase는 다음 iterator 반환
        }
        else
        {
            ++RigidBodyPair;
        }
    }

    /* 씬 시뮬레이션 */
    if (nullptr != m_PxScene)
    {
        m_PxScene->simulate(fTimeDelta);      // 시뮬레이션 시작
        m_PxScene->fetchResults(true);        // 결과 가져오기, PVD에 전송됨

        /* 위치 동기화 */
        for (auto& Pair : m_RigidBodies)
        {
            CTransform* pTransform = Pair.first->GetTransform();
            PxTransform pPxTransform = Pair.second->Get_PxTransform();

            const PxVec3& vPxPosition = pPxTransform.p;
            pTransform->Set_State(STATE::POSITION, XMVectorSet(vPxPosition.x, vPxPosition.y, vPxPosition.z, 1.f));

            const PxQuat& vPxRotation = pPxTransform.q;
            pTransform->Rotation(vPxRotation.x, vPxRotation.y, vPxRotation.z, vPxRotation.w);

            pTransform->Update_PreWorldMatrix();
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
    m_RigidBodies.clear();

    m_CCTs.clear();

    for (auto& Terrain : m_pTerrains)
    {
        m_PxScene->removeActor(*Terrain);
        Terrain->release();
    }

    m_pTerrains.clear();
}

HRESULT CPhysx_Manager::Add_CCT_ToPhysx(CGameObject* pGameObject, CCharacterController* pCCT)
{
    if (nullptr == pGameObject || nullptr == pCCT)
        return E_FAIL;

    //이건 SetOwner 되면 세팅
    //if (pCCT->GetOwner() != pGameObject)
    //    return E_FAIL;

    m_CCTs.push_back(make_pair(pGameObject, pCCT));

    Safe_AddRef(pGameObject);
    Safe_AddRef(pCCT);

    return S_OK;
}

HRESULT CPhysx_Manager::Add_RigidBody_ToPhysx(CGameObject* pGameObject, CRigidBody* pRigidBody)
{
    if (nullptr == pGameObject || nullptr == pRigidBody)
        return E_FAIL;

    //이건 SetOwner 되면 세팅
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
    //예외 처리
    if (nullptr == pTerrainVIBuffer)
        return E_FAIL;

    _uint iNumVerticesX = pTerrainVIBuffer->Get_NumVerticesX();
    _uint iNumVerticesZ = pTerrainVIBuffer->Get_NumVerticesZ();
    _float* pHeightData = pTerrainVIBuffer->Get_HeightData();

    //터레인 정점 갯수만큼 선언.
    PxHeightFieldSample* pSamples = new PxHeightFieldSample[iNumVerticesX * iNumVerticesZ];

    //점심먹고 터레인 추가하는 로직 작성

    for (_uint iRow = 0; iRow < iNumVerticesZ; ++iRow)
    {
        for (_uint iCol = 0; iCol < iNumVerticesX; ++iCol)
        {
            // Terrain heightData = height[z][x]
            // PhysX heightField  = sample[row][col]

            // X/Z swap: height[x][z]
            _uint srcIdx = iCol * iNumVerticesZ + iRow;           // 스왑된 소스 인덱스
            _uint dstIdx = iRow * iNumVerticesX + iCol;           // PhysX가 요구하는 인덱스


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

    // 씬 등록
    m_PxScene->addActor(*hfActor);
    hfActor->setName("TERRAIN");
    
    m_pTerrains.push_back(hfActor);

    pHeightField->release();
    Safe_Delete_Array(pHeightData);
    Safe_Delete_Array(pSamples);

    return S_OK;
}

PxTransform CPhysx_Manager::Convert_Matrix_ToPxTransform(_matrix WorldMatrix)
{
    /* 월드매트릭스 PxTransform으로 변경. */
    _vector vScale, vRotation, vPos;

    XMMatrixDecompose(&vScale ,&vRotation, &vPos, WorldMatrix);

    PxVec3 vPxPosition = PxVec3(XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos));
    PxQuat vPxQuaternion = PxQuat(XMVectorGetX(vRotation), XMVectorGetY(vRotation), XMVectorGetZ(vRotation), XMVectorGetW(vRotation));

    return PxTransform(vPxPosition, vPxQuaternion);
}

_matrix CPhysx_Manager::Convert_PxTransform_ToMatrix(PxTransform Transform)
{
    // 위치
    PxVec3  vPxPosition = Transform.p;
    _vector vPosition = XMVectorSet(vPxPosition.x, vPxPosition.y, vPxPosition.z, 1.0f);

    // 회전 (Quaternion)
    PxQuat  vPxQuaternion = Transform.q;
    _matrix vQuaternion = XMMatrixRotationQuaternion(XMVectorSet(vPxQuaternion.x, vPxQuaternion.y, vPxQuaternion.z, vPxQuaternion.w));

    // 스케일은 1로
    _vector vScale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

    // 행렬 생성
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