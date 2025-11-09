#include "Physx_Manager.h"

#include "GameObject.h"
#include "Engine_Defines.h"

CPhysx_Manager::CPhysx_Manager()
{
}

HRESULT CPhysx_Manager::Initialize()
{
    /* 피직스 초기화 */
	m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocator, m_DefaultErrorCallback);

    m_Pvd = physx::PxCreatePvd(*m_PxFoundation);

    m_PxTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_Pvd->connect(*m_PxTransport, physx::PxPvdInstrumentationFlag::eALL);

    m_PxPhysx = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, physx::PxTolerancesScale(), true, m_Pvd);
	if (nullptr == m_PxPhysx)
		return E_FAIL;

    PxInitExtensions(*m_PxPhysx, m_Pvd);

    physx::PxSceneDesc sceneDesc(m_PxPhysx->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

    m_PxDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

    sceneDesc.cpuDispatcher = m_PxDispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    m_PxScene = m_PxPhysx->createScene(sceneDesc);

    physx::PxPvdSceneClient* pvdClient = m_PxScene->getScenePvdClient();
    
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    TestSetting();

    return S_OK;
}

void CPhysx_Manager::Update(_float fTimeDelta)
{
    if (nullptr != m_PxScene)
    {
        m_PxScene->simulate(fTimeDelta);       // 시뮬레이션 시작
        m_PxScene->fetchResults(true);        // 결과 가져오기, PVD에 전송됨
    }
}

void CPhysx_Manager::TestSetting()
{
    physx::PxMaterial* mMaterial = NULL;
    // create simulation
    mMaterial = m_PxPhysx->createMaterial(0.5f, 0.5f, 0.6f);
    physx::PxRigidStatic* groundPlane = PxCreatePlane(*m_PxPhysx, physx::PxPlane(0, 1, 0, 50), *mMaterial);
    m_PxScene->addActor(*groundPlane);

    float halfExtent = .5f;
    physx::PxShape* shape = m_PxPhysx->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *mMaterial);
    physx::PxU32 size = 30;
    physx::PxTransform t(physx::PxVec3(0));
    for (physx::PxU32 i = 0; i < size; i++) {
        for (physx::PxU32 j = 0; j < size - i; j++) {
            physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * halfExtent);
            physx::PxRigidDynamic* body = m_PxPhysx->createRigidDynamic(t.transform(localTm));
            body->attachShape(*shape);
            physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
            m_PxScene->addActor(*body);
        }
    }
    shape->release();
}

void CPhysx_Manager::Clear()
{
    for (auto& Pair : m_DynamicActors)
       Pair.second->release();

    m_DynamicActors.clear();

    for (auto& Geometry : m_Geometries)
        Safe_Delete(Geometry);

    m_Geometries.clear();

    for (auto& TriangleMesh : m_TriangleMeshes)
        TriangleMesh->release();

    m_TriangleMeshes.clear();
}

void CPhysx_Manager::Add_GameObject_ToPhysx(CGameObject* pGameObject)
{
    /* 객체의 트랜스폼을 받아와서, 피직스에서 쓸 수 있는 상태로 변환한다. */
    
    /* 
    공식 문서 상에선 피직스 상의 단위 개념은 따로 없고, 
    사용자가 가져다 쓸때 하나의 단위계로만 통일해서 사용하면 
    문제 없을거라고 했음.
    */
    _matrix matWorld = XMLoadFloat4x4(pGameObject->GetTransform()->Get_WorldMatrixPtr());
	_vector vPos, vRotation, vScale;

    XMMatrixDecompose(&vPos, &vRotation, &vScale, matWorld);

    PxVec3 vPxPosition = PxVec3(XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos));
    PxQuat vPxQuaternion = PxQuat(XMVectorGetX(vRotation), XMVectorGetY(vRotation), XMVectorGetZ(vRotation), XMVectorGetW(vRotation));

	PxTransform PxWorldMatrix = PxTransform(vPxPosition, vPxQuaternion);
    PxRigidDynamic* pDynamicActor = m_PxPhysx->createRigidDynamic(PxWorldMatrix);

    /* 머테리얼은 튜토리얼 대로 세팅 */
    PxMaterial* Material = m_PxPhysx->createMaterial(0.5f, 0.5f, 0.6f);
    /* 0.5 반지름 (구), 0.5 반높이 (원통) 크기의 캡슐 콜라이더 세팅. */
    PxShape* pShape = m_PxPhysx->createShape(PxCapsuleGeometry(0.5f, 0.5f), *Material);

    pDynamicActor->attachShape(*pShape);
    ///* 연속 충돌 활성화. 현재는 플레이어만 제어하므로 켜도 된다. */
    //pDynamicActor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);

    pShape->release();
    Material->release();

    m_DynamicActors.push_back(make_pair(pGameObject, pDynamicActor));
    m_IsOnSlope.push_back(false);

    m_PxScene->addActor(*pDynamicActor);
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

    PxCloseExtensions();
    
    for (auto& Pair : m_DynamicActors)
        Pair.second->release();
    
    for (auto& Geometry : m_Geometries)
        Safe_Delete(Geometry);

    for (auto& TriangleMesh : m_TriangleMeshes)
        TriangleMesh->release();

    if (nullptr != m_PxScene)      
        m_PxScene->release();

    if (nullptr != m_PxDispatcher)
        m_PxDispatcher->release();

    if (nullptr != m_PxPhysx)
        m_PxPhysx->release();

    if (nullptr != m_Pvd)
        m_Pvd->release();

    if (nullptr != m_PxTransport)
        m_PxTransport->release();

    if (nullptr != m_PxFoundation)
        m_PxFoundation->release();
}


/* 지형 처리할 오브젝트 내부에서 Initialize 할때 수행 */
//void CPhysx_Manager::Add_Geometry_ToPhysx(CGameObject* pGameObject, CModel* pModel)
//{
//    _int iMeshNum = pModel->Get_MeshNum();
//
//    for (_int i = 0; i < iMeshNum; ++i)
//    {
//        CMesh* pMesh = pModel->Get_Mesh(i);
//        _float3* pVertexPositions = pMesh->Get_VertexPositions();
//        _uint* pIndices = pMesh->Get_Indices();
//        _uint iNumVertices = pMesh->Get_NumVertices();
//        _uint iNumIndices = pMesh->Get_NumIndices();
//        _uint iNumPrimitives = iNumIndices / 3;
//
//        vector<PxVec3> Vertices;
//        Vertices.reserve(iNumVertices);
//
//        _matrix matWorld = XMLoadFloat4x4(pGameObject->Get_Transform()->Get_WorldMatrixPtr());
//
//        /* 정점 정보 뽑아오는 과정 */
//        for (_uint i = 0; i < iNumVertices; ++i)
//        {
//            /* 로컬 좌표 뽑아옴 */
//            _vector vMeshLocalPos;
//            vMeshLocalPos = XMLoadFloat3(&pVertexPositions[i]);
//            /* 뽑아온 로컬 좌표 기반으로 월드 좌표로 올림 */
//            _vector vMeshWorldPos = XMVector3TransformCoord(vMeshLocalPos, matWorld);
//
//            /* Vertices 행렬에 PxVec3 형태로 담아줌 */
//            Vertices.push_back(PxVec3(XMVectorGetX(vMeshWorldPos), XMVectorGetY(vMeshWorldPos), XMVectorGetZ(vMeshWorldPos)));
//        }
//
//        /* 인덱스 정보 뽑아오는 과정 */
//        vector<PxU32> Indices;
//        Indices.reserve(iNumIndices);
//
//        _uint iIndex = { 0 };
//
//        for (_uint i = 0; i < iNumPrimitives; ++i)
//        {
//            Indices.push_back(pIndices[iIndex++]);
//            Indices.push_back(pIndices[iIndex++]);
//            Indices.push_back(pIndices[iIndex++]);
//        }
//
//        PxTriangleMeshDesc Desc;
//
//        Desc.points.count = iNumVertices;
//        Desc.points.stride = sizeof(PxVec3);
//        Desc.points.data = Vertices.data();
//
//        Desc.triangles.count = iNumPrimitives;
//        Desc.triangles.stride = sizeof(PxU32) * 3;
//        Desc.triangles.data = Indices.data();
//
//
//        PxTriangleMesh* pTriangleMesh = PxCreateTriangleMesh(PxCookingParams(PxTolerancesScale(0.0f, 0.0f)), Desc);
//        PxTriangleMeshGeometry* pGeometry = new PxTriangleMeshGeometry(pTriangleMesh);
//
//        /* 지형 완성은 여기서 끝 */
//        m_TriangleMeshes.push_back(pTriangleMesh);
//        m_Geometries.push_back(pGeometry);
//    }
//}
//
//void CPhysx_Manager::Calc_Geometry()
//{
//    Check_GeometryCollision();
//    Check_GeometryPicking();
//}
//
//_bool CPhysx_Manager::Check_GeometryCollision()
//{
//    /* Triangle Mesh Geometry와 충돌 처리 */
//    _bool IsGround = false;
//    _uint iIdx = 0;
//
//    for (auto& Pair : m_DynamicActors)
//    {
//        /* 계산 하기 전, 트랜스폼 가져와서 actor 최신화. */
//        _matrix matWorld = XMLoadFloat4x4(Pair.first->Get_Transform()->Get_WorldMatrixPtr());
//        _vector vTranslation, vRotation, vScale;
//
//        XMMatrixDecompose(&vScale, &vRotation, &vTranslation, matWorld);
//
//        PxVec3 vPxPosition = PxVec3(XMVectorGetX(vTranslation), XMVectorGetY(vTranslation), XMVectorGetZ(vTranslation));
//        PxQuat vPxQuaternion = PxQuat(XMVectorGetX(vRotation), XMVectorGetY(vRotation), XMVectorGetZ(vRotation), XMVectorGetW(vRotation));
//
//        PxTransform PxWorldMatrix = PxTransform(vPxPosition, vPxQuaternion);
//        Pair.second->setGlobalPose(PxWorldMatrix);
//
//        /* Actor에서 현재 위치 / 회전 가져오기 */
//        PxTransform ActorTransform = Pair.second->getGlobalPose();
//
//        /* 캡술 콜라이더 정보 꺼내오기 */
//        PxShape* pShape = nullptr;
//        Pair.second->getShapes(&pShape, 1);
//
//        PxCapsuleGeometry Geom;
//        PxGeometryHolder GeomHolder = pShape->getGeometry(); 
//        if (GeomHolder.getType() == PxGeometryType::eCAPSULE)
//            Geom = GeomHolder.capsule(); 
//
//        /* 월드 기준 캡슐 콜라이더 정보 계산 */
//        PxVec3 vPxUp = ActorTransform.q.rotate(PxVec3(0, 1, 0));
//        PxVec3 vPxStartPos = ActorTransform.p + vPxUp * -Geom.halfHeight;
//        PxVec3 vPxEndPos = ActorTransform.p + vPxUp * Geom.halfHeight;
//
//        for (auto& Mesh : m_Geometries)
//        {
//            PxVec3 vDir;
//            PxReal fLength;
//            if (PxComputeTriangleMeshPenetration(vDir, fLength, Geom, ActorTransform, *Mesh, PxTransform(PxIDENTITY::PxIdentity), 1))
//            {
//                _vector vResultDir = XMVectorSet(vDir.x, vDir.y, vDir.z, 0.f);
//                vResultDir = XMVector3Normalize(vResultDir);
//
//                // 땅 체크
//                _vector vVertical = XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);
//                _float fAngle = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vVertical, vResultDir))));
//                if (fAngle < 60.f)
//                    IsGround = true;
//                else
//                {
//                    /* 경사면에 있었다면 지형과의 충돌처리는 하지 않는다. */
//                    if (m_IsOnSlope[iIdx] == true)
//                    {
//                        m_IsOnSlope[iIdx] = false;
//                        continue;
//                    }
//                    else
//                    {
//                        XMVectorSetY(vResultDir, 0.f);
//                    }
//                }
//
//                vResultDir *= fLength;
//
//                // 플레이어 Transform 위치 보정
//                _vector vOriginPos = Pair.first->Get_Transform()->Get_State(STATE::POSITION);
//                _vector vResultPos = vOriginPos + vResultDir;
//                Pair.first->Get_Transform()->Set_State(STATE::POSITION, vResultPos);
//
//            }
//        }
//        iIdx++;
//    }
//
//    /* 
//    모든 메쉬와의 충돌 처리는 어쨌든 수행하긴 해야 한다. 
//    (2곳 이상 동시에 충돌하는 상황도 있고 여러 예외 사항이 많음.)
//    */
//    return IsGround;
//}
//
//_bool CPhysx_Manager::Check_GameObject_GeometryCollision(CGameObject* pGameObject, _bool* IsCollision)
//{
//    /* Triangle Mesh Geometry와 충돌 처리 */
//    _bool IsGround = false;
//
//    if (nullptr != IsCollision)
//        *IsCollision = false;
//
//    for (auto& Pair : m_DynamicActors)
//    {
//        /* 계산 하기 전, 트랜스폼 가져와서 actor 최신화. */
//        /* 특정 오브젝트만 충돌처리 해주는 함수. 주소 기반으로 비교하게 된다. */
//        
//        if (Pair.first == pGameObject)
//        {
//            _matrix matWorld = XMLoadFloat4x4(Pair.first->Get_Transform()->Get_WorldMatrixPtr());
//            _vector vTranslation, vRotation, vScale;
//
//            XMMatrixDecompose(&vScale, &vRotation, &vTranslation, matWorld);
//
//            PxVec3 vPxPosition = PxVec3(XMVectorGetX(vTranslation), XMVectorGetY(vTranslation), XMVectorGetZ(vTranslation));
//            PxQuat vPxQuaternion = PxQuat(XMVectorGetX(vRotation), XMVectorGetY(vRotation), XMVectorGetZ(vRotation), XMVectorGetW(vRotation));
//
//            PxTransform PxWorldMatrix = PxTransform(vPxPosition, vPxQuaternion);
//            Pair.second->setGlobalPose(PxWorldMatrix);
//
//            /* Actor에서 현재 위치 / 회전 가져오기 */
//            PxTransform ActorTransform = Pair.second->getGlobalPose();
//
//            /* 캡술 콜라이더 정보 꺼내오기 */
//            PxShape* pShape = nullptr;
//            Pair.second->getShapes(&pShape, 1);
//
//            PxCapsuleGeometry Geom;
//            PxGeometryHolder GeomHolder = pShape->getGeometry();
//            if (GeomHolder.getType() == PxGeometryType::eCAPSULE)
//                Geom = GeomHolder.capsule();
//
//            /* 월드 기준 캡슐 콜라이더 정보 계산 */
//            PxVec3 vPxUp = ActorTransform.q.rotate(PxVec3(0, 1, 0));
//            PxVec3 vPxStartPos = ActorTransform.p + vPxUp * -Geom.halfHeight;
//            PxVec3 vPxEndPos = ActorTransform.p + vPxUp * Geom.halfHeight;
//
//            for (auto& Mesh : m_Geometries)
//            {
//                PxVec3 vDir;
//                PxReal fLength;
//                
//                if (PxComputeTriangleMeshPenetration(vDir, fLength, Geom, ActorTransform, *Mesh, PxTransform(PxIDENTITY::PxIdentity), 1))
//                {
//                    if (nullptr != IsCollision)
//                        *IsCollision = true;
//                    _vector vResultDir = XMVectorSet(vDir.x, vDir.y, vDir.z, 0.f);
//                    vResultDir = XMVector3Normalize(vResultDir);
//
//                    // 땅 체크
//                    _vector vVertical = XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);
//                    _float fAngle = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vVertical, vResultDir))));
//                    
//                    if (fAngle < 60.f)
//                        IsGround = true;
//
//                    else
//                        XMVectorSetY(vResultDir, 0.f);
//
//                    vResultDir *= fLength;
//
//                    // 플레이어 Transform 위치 보정
//                    _vector vOriginPos = Pair.first->Get_Transform()->Get_State(STATE::POSITION);
//                    _vector vResultPos = vOriginPos + vResultDir;
//                    Pair.first->Get_Transform()->Set_State(STATE::POSITION, vResultPos);
//                }
//            }
//
//            return IsGround;
//        }
//    }
//
//    /*
//    모든 메쉬와의 충돌 처리는 어쨌든 수행하긴 해야 한다.
//    (2곳 이상 동시에 충돌하는 상황도 있고 여러 예외 사항이 많음.)
//    */
//    return false;
//}
//
///*
//아이온 퓨리때 쓴 방법이긴 한데.. 먹힐진 모르겠다. 
//
//0, -1, 0 의 방향을 가지는 레이를 쏜 뒤,
//1. 거리에 따라 지형에 달라붙게 할지, 2. 떨어지게 할지 처리하는 함수
//*/
//_bool CPhysx_Manager::Check_GeometryPicking()
//{
//    /*
//    origin : 레이의 시작점 ( 플레이어의 위치 )
//    unitDir : 레이의 방향을 정의하는 단위 벡터 ( 무조건 0,-1,0 임. 이미 월드상 위치로 세팅 되어있기 때문이다. )
//    maxDist : 레이를 따라 검색할 최대 거리 (0~inf 범위 내에 있어야 함) ( 임의로 100으로 설정 )
//    geom : 테스트할 Geometry ( Geometry 순회 ) 
//    pose : geom의 위치 -> 임의로 항등행렬로 세팅.  
//    hitFlags : 쿼리에서 반환해야 할 값을 지정하고 쿼리를 처리하는 옵션
//    maxHits : 반환할 최대 충돌 수 ( 1 )
//    hitInfo : PxRaycastHit 구조체를 받으며, 레이캐스트 결과 저장
//    */
//
//    /* 
//    eMeshAny 플래그를 사용하지 않는다면, 
//    가장 가까운 위치의 포인트를 자동으로 반환해준다고 한다.
//    */
//
//    PxVec3 vPxPosition = PxVec3(0.f, 0.f, 0.f);
//    PxQuat vPxQuaternion = PxQuat(0.f, 0.f, 0.f, 1.f);
//    _uint iIdx = { 0 };
//
//    PxTransform PxMeshWorldMatrix = PxTransform(vPxPosition, vPxQuaternion);
//
//    for (auto& Pair : m_DynamicActors)
//    {
//        if (false == Pair.first->Get_Pickable())
//            continue;
//
//        /* 계산 하기 전, 트랜스폼 가져와서 actor 최신화. */
//        _matrix matWorld = XMLoadFloat4x4(Pair.first->Get_Transform()->Get_WorldMatrixPtr());
//        _vector vTranslation, vRotation, vScale;
//
//        XMMatrixDecompose(&vScale, &vRotation, &vTranslation, matWorld);
//
//        PxVec3 vPxPosition = PxVec3(XMVectorGetX(vTranslation), XMVectorGetY(vTranslation), XMVectorGetZ(vTranslation));
//        PxQuat vPxQuaternion = PxQuat(XMVectorGetX(vRotation), XMVectorGetY(vRotation), XMVectorGetZ(vRotation), XMVectorGetW(vRotation));
//
//        PxTransform PxWorldMatrix = PxTransform(vPxPosition, vPxQuaternion);
//        Pair.second->setGlobalPose(PxWorldMatrix);
//        /* Actor에서 현재 위치 / 회전 가져오기 */
//        PxTransform ActorTransform = Pair.second->getGlobalPose();
//
//        _float fNearestDist = { FLT_MAX };
//        _vector vNearestPos = {};
//        _vector vGroundNormal = {};
//
//        for (auto& Mesh : m_Geometries)
//        {
//            PxRaycastHit HitInfo = PxRaycastHit();
//
//            /* 히트 지점 갯수를 반환한다. 즉, 0개라면 충돌이 존재하지 않음. */
//            PxU32 HitCount = PxGeometryQuery::raycast(
//                ActorTransform.p,       // 레이 위치 
//                PxVec3(0.f, -1.f, 0.f), // 레이 방향
//                *Mesh,                  // Geometry 정보
//                PxMeshWorldMatrix,          // Geometry의 트랜스폼 가져와야 함.
//                100.f,                  // 체크할 최대 거리
//                PxHitFlags(PxHitFlag::ePOSITION | PxHitFlag::eNORMAL),   // 플래그, (기본적으로 Distance는 제공. Position & Normal 까지 추가로 가져옴)
//                1,                      // 체크할 최대 히트 갯수 ( 1개라면 가장 가까운 피킹 지점의 정보 반환 )
//                &HitInfo);
//
//            if (0 != HitCount) 
//            {
//                _vector vResultPos = XMVectorSet(HitInfo.position.x, HitInfo.position.y, HitInfo.position.z, 1.f);
//                _float  fDist = HitInfo.distance;
//                /* 거리가 0.1보다 짧다면, 달라붙게끔 한다.*/
//                /* 콜라이더의 중심으로부터 세팅되므로, 현재 콜라이더 크기인 
//                0.5 반구, 0.5 반 높이를 가진 캡슐 콜라이더임을 명심해야함. */
//                if (fDist <= fNearestDist)
//                {
//                    vNearestPos = vResultPos;
//                    fNearestDist = fDist;
//                    vGroundNormal = XMVector3Normalize(XMVectorSet(HitInfo.normal.x, HitInfo.normal.y, HitInfo.normal.z, 0.f));
//                }
//            }
//        }
//
//        PxShape* shape = nullptr;
//        Pair.second->getShapes(&shape, 1); // 첫 번째 shape 가져오기
//
//        PxCapsuleGeometry geom;
//        PxGeometryHolder holder = shape->getGeometry();
//        if (holder.getType() == PxGeometryType::eCAPSULE)
//            geom = holder.capsule(); // 런타임 캡슐 정보 가져오기
//
//        // 만약 경사진 면이라면 그라운드 벡터 사용, 그게 아니라면 그냥 보정. 
//        _vector vVertical = XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);
//        _float fAngle = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vVertical, vGroundNormal))));
//
//        /* 바로 땅에 붙이기 */
//        if (fNearestDist <= 0.3f)
//        {
//            return true;
//        }
//        /* 경사에 따른 지형 처리를 다르게 세팅. */
//        else if (fNearestDist <= 1.5f && fAngle < 25.f)
//        {
//            Pair.first->Get_Transform()->Set_State(STATE::POSITION, vNearestPos + XMVectorSet(0.f, 0.7f, 0.f, 0.f));
//        }
//        else if (fNearestDist <= 1.8f && fAngle >= 25.f) 
//        {
//            Pair.first->Get_Transform()->Set_State(STATE::POSITION, vNearestPos + XMVectorSet(0.f, 1.0f, 0.f, 0.f));
//        }
//        else if (fNearestDist <= 2.0f && fAngle >= 40.f)
//        {
//            Pair.first->Get_Transform()->Set_State(STATE::POSITION, vNearestPos + XMVectorSet(0.f, 1.2f, 0.f, 0.f));
//        }
//        else if (fNearestDist <= 2.2f && fAngle >= 60.f)
//        {
//            Pair.first->Get_Transform()->Set_State(STATE::POSITION, vNearestPos + XMVectorSet(0.f, 1.4f, 0.f, 0.f));
//            m_IsOnSlope[iIdx] = true;
//        }
//        else if (fNearestDist <= 2.3f && fAngle >= 70.f)
//        {
//            Pair.first->Get_Transform()->Set_State(STATE::POSITION, vNearestPos + XMVectorSet(0.f, 1.5f, 0.f, 0.f));
//            m_IsOnSlope[iIdx] = true;
//        }
//        else if(fNearestDist <= 1.5f && fAngle >= 80.f)
//        {
//            Pair.first->Set_Gravity(true, fNearestDist);
//        }
//        /* 가볍게 떨어지게 한다. */
//        else
//        {
//            Pair.first->Set_Gravity(true, fNearestDist);
//        }
//    }
//
//    return true;
//}
//
///* 주어진 레이와 충돌한 결과를 가져온다. */
//_bool CPhysx_Manager::Check_Ray_GeometryPicking(_float3 vRayPos, _float3 vRayDir, _float3* vResultPos, _float* fResultDist)
//{
//    /* 레이 트랜스폼 생성 */
//    PxVec3 vPxRayPos = PxVec3(vRayPos.x, vRayPos.y, vRayPos.z);
//    PxQuat vPxRayQuaternion = PxQuat(0.f, 0.f, 0.f, 1.f);
//
//    PxTransform PxRayWorldMatrix = PxTransform(vPxRayPos, vPxRayQuaternion);
//
//    /* 충돌용 메시 트랜스폼 생성 */
//    PxVec3 vPxPosition = PxVec3(0.f, 0.f, 0.f);
//    PxQuat vPxQuaternion = PxQuat(0.f, 0.f, 0.f, 1.f);
//
//    PxTransform PxMeshWorldMatrix = PxTransform(vPxPosition, vPxQuaternion);
//
//    _float fNearestDist = { FLT_MAX };
//    _vector vNearestPos = {};
//
//    for (auto& Mesh : m_Geometries)
//    {
//        PxRaycastHit HitInfo = PxRaycastHit();
//
//        /* 히트 지점 갯수를 반환한다. 즉, 0개라면 충돌이 존재하지 않음. */
//        PxU32 HitCount = PxGeometryQuery::raycast(
//            PxRayWorldMatrix.p,       // 레이 위치 
//            PxVec3(vRayDir.x, vRayDir.y, vRayDir.z), // 레이 방향
//            *Mesh,                  // Geometry 정보
//            PxMeshWorldMatrix,          // Geometry의 트랜스폼 가져와야 함.
//            100.f,                  // 체크할 최대 거리
//            PxHitFlags(PxHitFlag::ePOSITION),   // 플래그, (기본적으로 Distance는 제공. Position까지 추가로 가져옴)
//            1,                      // 체크할 최대 히트 갯수 ( 1개라면 가장 가까운 피킹 지점의 정보 반환 )
//            &HitInfo);
//
//        if (0 != HitCount)
//        {
//            _vector vResultPos = XMVectorSet(HitInfo.position.x, HitInfo.position.y, HitInfo.position.z, 1.f);
//            _float  fDist = HitInfo.distance;
//
//            if (fDist <= fNearestDist)
//            {
//                vNearestPos = vResultPos;
//                fNearestDist = fDist;
//            }
//        }
//    }
//
//    XMStoreFloat3(vResultPos, vNearestPos);
//    *fResultDist = fNearestDist;
//
//    return true;
//}