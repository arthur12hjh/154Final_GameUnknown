#include "JointChain.h"

#include "GameInstance.h"
#include "RigidBody.h"

CJointChain::CJointChain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CJointChain::CJointChain(const CJointChain& Prototype)
    : CComponent{ Prototype }
{
}

HRESULT CJointChain::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CJointChain::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CJointChain::Set_Root(CRigidBody* pRigidBody)
{
    if (nullptr == pRigidBody || nullptr != m_pRoot)
        return E_FAIL;

    m_pRoot = pRigidBody;
    Safe_AddRef(m_pRoot);

    return S_OK;
}

//조인트 세팅 건드리면 죽을게요 ㅇㅇ
HRESULT CJointChain::Add_Joint(CRigidBody* pRigidBody)
{
    Set_ChildJoint(pRigidBody);

    if (nullptr == pRigidBody)
        return E_FAIL;

    CRigidBody* pParentRigidBody = (m_iNumJoints == 0) ? m_pRoot : m_RigidBodies.back();
    if (!pParentRigidBody)
        return E_FAIL;

    PxRigidActor* pParentActor = pParentRigidBody->Get_PxRigidBody();
    PxRigidActor* pChildActor  = pRigidBody->Get_PxRigidBody();
    if (!pParentActor || !pChildActor)
        return E_FAIL;

    PxVec3 vParentPos = pParentActor->getGlobalPose().p;
    PxVec3 vChildPos  = pChildActor->getGlobalPose().p;
    PxVec3 vDiff      = vChildPos - vParentPos;
    _float fLength    = vDiff.magnitude(); // 두 액터 중심 간 거리

    // ####################축 정렬
    PxVec3 vDir = vDiff.getNormalized();

    // 앵커 위치 설정 (중간 지점에서 만난다고 가정)
    // 반지름 변수(fParentRadius)가 없다면 중심 거리의 절반씩 나누어 설정
    _float fHalfLen = fLength * 0.5f;

    // D6 조인트 생성 (기존 DistanceJoint 코드 대체)
    // PxTransform(위치, 회전) : 위치는 로컬 오프셋, 회전은 축 정렬
    PxD6Joint* pJoint = PxD6JointCreate(
        *m_pGameInstance->Get_PxPhysics(),
        pParentActor, PxTransform(PxIdentity),
        pChildActor,  PxTransform(PxIdentity));

    // 월드 앵커: 두 노드의 중점
    PxVec3 WorldAnchor = (vParentPos + vChildPos) * 0.5f;
    // 월드 축: 부모->자식 방향(조인트 기준축)
    PxVec3 WorldAnchorAxis = vDir;
    // 프레임을 월드 기준으로 세팅 (localFrame0/1 자동 계산)
    PxVec3 WorldAixs = vDir;
    PxVec3 WorldUp = PxVec3(0.f, 1.f, 0.f);

    if (PxAbs(WorldUp.dot(WorldAixs)) > 0.95f)
        WorldUp = PxVec3(0.f, 0.f, 1.f);

    // axis(X), normal(Y)까지 같이 고정
    PxSetJointGlobalFrame(*pJoint, &WorldAnchor, &WorldAixs);

    if (!pJoint)
        return E_FAIL;

    // 모션 제한 설정
    // 길이는 고정
    pJoint->setMotion(PxD6Axis::eX, PxD6Motion::eLOCKED);
    pJoint->setMotion(PxD6Axis::eY, PxD6Motion::eLOCKED);
    pJoint->setMotion(PxD6Axis::eZ, PxD6Motion::eLOCKED);

    // 회전 설정
    // Twist(비틀림 X축), Swing(꺾임 Y,Z축)
    pJoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
    pJoint->setTwistLimit(PxJointAngularLimitPair(-PxPi / 18.f, PxPi / 18.f)); // ±15도

    pJoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
    pJoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);

    // 꺾임 각도 제한 (예: 45도 = PI/4)
    // 45 ~ 60도 제한
    PxJointLimitCone ConeLimit(PxPi / 12.f, PxPi / 4.f); // ~18도, contactDist 약간
    ConeLimit.restitution     = 0.f; // 튕김 제거
    ConeLimit.bounceThreshold = 0.f; // 튕김 임계 제거
    ConeLimit.stiffness = 0.f; // 리미트에 스프링 추가 안 함
    ConeLimit.damping   = 0.f; // 여기서도 감쇠 줄 수 있음
    pJoint->setSwingLimit(ConeLimit);

    // 스프링 설정 - 머리카락 찰랑거림
    // 원래 상태로 돌아오려는 힘
    pJoint->setDrive(
        PxD6Drive::eSLERP,
        PxD6JointDrive(
            150.f,       // stiffness
            18.f,        // damping
            80.f,  // forceLimit
            false         // isAcceleration
        )
    );

    pJoint->setDrivePosition(PxTransform(PxIdentity));
    pJoint->setDriveVelocity(PxVec3(0.f), PxVec3(0.f));

    // 충돌 끄기
    pJoint->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, false);

    pJoint->setInvMassScale0(0.f);
    pJoint->setInvInertiaScale0(0.f);
    pJoint->setInvMassScale1(1.0f);
    pJoint->setInvInertiaScale1(1.0f);

    m_RigidBodies.push_back(pRigidBody);
    Safe_AddRef(pRigidBody);

    m_Joints.push_back(pJoint); // m_Joints 컨테이너 타입 확인 필요!
    m_iNumJoints = (_uint)m_RigidBodies.size();

    return S_OK;
}

/*
HRESULT CJointChain::Add_Joint_Local(CRigidBody* pParent, CRigidBody* pChild, const PxTransform& tLocalPose)
{
    if (nullptr == pChild)
        return E_FAIL;

    static_cast<PxRigidDynamic*>(pChild->Get_PxRigidBody())->setSolverIterationCounts(24, 8);
    static_cast<PxRigidDynamic*>(pChild->Get_PxRigidBody())->setAngularDamping(0.9f);
    static_cast<PxRigidDynamic*>(pChild->Get_PxRigidBody())->setLinearDamping(0.09f);
    static_cast<PxRigidDynamic*>(pChild->Get_PxRigidBody())->setMaxAngularVelocity(15.f);
    static_cast<PxRigidDynamic*>(pChild->Get_PxRigidBody())->setMaxDepenetrationVelocity(2.0f);
    static_cast<PxRigidDynamic*>(pChild->Get_PxRigidBody())->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);

    if (nullptr == pParent)
        return E_FAIL;

    PxRigidActor* pParentActor = pParent->Get_PxRigidBody();
    PxRigidActor* pChildActor  = pChild->Get_PxRigidBody();
    if (!pParentActor || !pChildActor)
        return E_FAIL;

    PxD6Joint* pJoint = PxD6JointCreate(
        *m_pGameInstance->Get_PxPhysics(),
        pParentActor, tLocalPose,
        pChildActor,  PxTransform(PxIdentity));

    if (!pJoint)
        return E_FAIL;

    pJoint->setMotion(PxD6Axis::eX, PxD6Motion::eLOCKED);
    pJoint->setMotion(PxD6Axis::eY, PxD6Motion::eLOCKED);
    pJoint->setMotion(PxD6Axis::eZ, PxD6Motion::eLOCKED);

    pJoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
    pJoint->setTwistLimit(PxJointAngularLimitPair(-PxPi / 18.f, PxPi / 18.f));

    pJoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
    pJoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);

    PxJointLimitCone ConeLimit(PxPi / 6.f, PxPi / 6.f);
    ConeLimit.restitution     = 0.f;
    ConeLimit.bounceThreshold = 0.f;
    ConeLimit.stiffness       = 0.f;
    ConeLimit.damping         = 0.f;
    pJoint->setSwingLimit(ConeLimit);

    pJoint->setDrive(PxD6Drive::eSLERP,
        PxD6JointDrive(60.f, 10.f, 20.f, true));

    pJoint->setDrivePosition(PxTransform(PxIdentity));
    pJoint->setDriveVelocity(PxVec3(0.f), PxVec3(0.f));

    pJoint->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, false);

    pJoint->setInvMassScale0(0.5f);
    pJoint->setInvInertiaScale0(0.5f);
    pJoint->setInvMassScale1(1.0f);
    pJoint->setInvInertiaScale1(1.0f);

    m_RigidBodies.push_back(pChild);
    Safe_AddRef(pChild);

    m_Joints.push_back(pJoint);
    m_iNumJoints = (_uint)m_RigidBodies.size();

    return S_OK;
}
*/

void CJointChain::Update(_float fTimeDelta)
{
    Set_RootJoint();

    for (_uint i = 0; i < m_iNumJoints; ++i)
    {
        Set_ChildJoint(m_RigidBodies[i]);
    }
}

void CJointChain::Set_RootJoint()
{
    //   - 과도하게 높일 필요는 없고, 흔히 16~24 / 4~8 범위에서 타협
    static_cast<PxRigidDynamic*>(m_pRoot->Get_PxRigidBody())->setSolverIterationCounts(24, 8); // posIters/velIters (일단 강하게)
    //   - "너무 금방 멈춘다/찰랑이 없다" -> AngularDamping 내리기 (예: 2.0 -> 1.0 ~ 1.5)
    //   - "상모돌리기/과회전"           -> AngularDamping 올리기 (예: 2.0 -> 3.0 ~ 4.0)
    static_cast<PxRigidDynamic*>(m_pRoot->Get_PxRigidBody())->setAngularDamping(1.9f);
    // - 머리카락처럼 "가볍게" 보이려면 보통 낮게 둠(0.05~0.3 선에서 많이 시작)
    static_cast<PxRigidDynamic*>(m_pRoot->Get_PxRigidBody())->setLinearDamping(0.025f);
    //   - "돌아오는게 느리다/답답" -> MaxAngularVelocity 올리기 (예: 8 -> 15~30)
    //   - "너무 과하게 휙휙 돈다" -> MaxAngularVelocity 내리기 (예: 30 -> 15)
    static_cast<PxRigidDynamic*>(m_pRoot->Get_PxRigidBody())->setMaxAngularVelocity(10.f);
    //   - "몸에 닿으면 질질 끌리고 느리다" -> MaxDepenetrationVelocity 올리기 (예: 2 -> 6~12)
    //   - "충돌 때 튕겨나가며 과장"        -> MaxDepenetrationVelocity 내리기 (예: 12 -> 6)
    static_cast<PxRigidDynamic*>(m_pRoot->Get_PxRigidBody())->setMaxDepenetrationVelocity(10.f); // 충돌 보정 폭주 억제
    static_cast<PxRigidDynamic*>(m_pRoot->Get_PxRigidBody())->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
}

void CJointChain::Set_ChildJoint(CRigidBody* pRigidBody)
{
    static_cast<PxRigidDynamic*>(pRigidBody->Get_PxRigidBody())->setSolverIterationCounts(24, 8); // posIters/velIters (일단 강하게)
    static_cast<PxRigidDynamic*>(pRigidBody->Get_PxRigidBody())->setAngularDamping(1.9f);
    static_cast<PxRigidDynamic*>(pRigidBody->Get_PxRigidBody())->setLinearDamping(0.025f);
    static_cast<PxRigidDynamic*>(pRigidBody->Get_PxRigidBody())->setMaxAngularVelocity(10.f);
    static_cast<PxRigidDynamic*>(pRigidBody->Get_PxRigidBody())->setMaxDepenetrationVelocity(10.f); // 충돌 보정 폭주 억제
    static_cast<PxRigidDynamic*>(pRigidBody->Get_PxRigidBody())->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
}

CJointChain* CJointChain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CJointChain* pInstance = new CJointChain(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Create Failed : JointChain");
    }

    return pInstance;
}

CComponent* CJointChain::Clone(void* pArg)
{
    CJointChain* pInstance = new CJointChain(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Clone Failed : JointChain");
    }

    return pInstance;
}

void CJointChain::Free()
{
    __super::Free();

    Safe_Release(m_pRoot);

    for (auto& iter : m_Joints)
        iter->release();
    m_Joints.clear();

    for (auto& iter : m_RigidBodies)
        Safe_Release(iter);
    m_RigidBodies.clear();
} 