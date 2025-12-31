#include "CharacterController.h"

#include "GameInstance.h"
#include "CCTHitReporter.h"
#include "CCTBehaviorCallback.h"
#include "CCTQueryFilterCallback.h"
#include "CCTFilterCallback.h"

#include "GameObject.h"

CCharacterController::CCharacterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
	, m_pPxPhysics	  { m_pGameInstance->Get_PxPhysics() }
	, m_pPxCCTManager { m_pGameInstance->Get_PxCCTManager() }
{
}

CCharacterController::CCharacterController(const CCharacterController& Prototype)
	: CComponent{ Prototype }
	, m_pPxPhysics	  { m_pGameInstance->Get_PxPhysics() }
	, m_pPxCCTManager { m_pGameInstance->Get_PxCCTManager() }
{
}

PxShape* CCharacterController::Get_PxShape()
{
	PxRigidDynamic* pPxActor = m_pController->getActor();
	PxShape* pPxShapes[1];
	PxU32 iNumShapes = pPxActor->getShapes(pPxShapes, 1); // 1은 배열 크기

	if (iNumShapes > 0)
	{
		PxShape* pPxShape = pPxShapes[0];
		return pPxShape;
	}
	else
		return nullptr;
}

PxRigidActor* CCharacterController::Get_PxActor()
{
	PxRigidDynamic* pPxActor = m_pController->getActor();

	return pPxActor;
}

void CCharacterController::Set_Gravity(_bool bFlag, _float fVelocity)
{
	//둘다 true거나 둘다 false면 넘어가
	m_isGravity = bFlag;
	m_fGravityTimeAcc = 0.f;
	m_fJumpVelocity = fVelocity;  // 이제 현재 속도로 사용
}

void CCharacterController::Set_Riding(_bool isRiding, CTransform* pTransform,  CRigidBody* pRigidBody)
{
	m_isRiding = isRiding;
	m_pRidingTarget = pRigidBody;
	m_pRidingTargetTransform = pTransform;
}

_float CCharacterController::Get_Radius()
{
	if (CCT_SHAPE::CAPSULE != m_eShape)
		return -1.f;

	return static_cast<PxCapsuleController*>(m_pController)->getRadius();
}

HRESULT CCharacterController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCharacterController::Initialize(void* pArg)
{
	CCT_DESC* pDesc = static_cast<CCT_DESC*>(pArg);
	pDesc->tUserData.isCCT = true;

	PxFilterData Filter;
	Filter.word0 = pDesc->iCollisionGroup;       // 예: (1<<5)
	Filter.word1 = pDesc->iCollisionMask;

	switch (pDesc->eCharacterControllerType)
	{
	case CCT_SHAPE::BOX:
		Ready_BoxController(pDesc);
		break;
	
	case CCT_SHAPE::CAPSULE:
		Ready_CapsuleController(pDesc);
		break;
	
	default:
		return E_FAIL;
	}

	Get_PxShape()->setSimulationFilterData(Filter);
	Get_PxShape()->setQueryFilterData(Filter);

	m_pCCTFilterCallback = CCTFilterCallback::Create();

	return S_OK;
}

void CCharacterController::Update_PrePxPosition(CTransform* pOwnerTransform)
{
	if (false == m_tUserData.isActive)
		return;
	///* 이전 프레임의 위치 갱신. */
	_float3 vPos;
	XMStoreFloat3(&vPos, pOwnerTransform->Get_State(STATE::POSITION));
	m_vPrePosition = PxVec3(vPos.x, vPos.y, vPos.z);
}

/* 캐릭터 컨트롤러는 시뮬레이션 말고 독자적으로 물리처리 해준다고 함..*/
void CCharacterController::Update_PxPosition(_float fTimeDelta, class CTransform* pOwnerTransform)
{
	if (nullptr == m_pController || false == m_tUserData.isActive)
		return;

	PxVec3 vTargetDeltaMove = { 0.f, 0.f, 0.f };

	if (true == m_isRiding)
	{

		if (false == m_pRidingTarget->IsRidable())
		{
			m_isRiding = false;
			m_isGravity = true;
			m_pRidingTarget = nullptr;
			m_iFrameCounter = 0;
		}

		else if (true == m_pRidingTarget->IsRidable())
		{
			vTargetDeltaMove = m_pRidingTarget->Get_DeltaMove();
			m_isGravity = false;
		}
	}

	//중력은 Transform에 적용.
	_vector vGravity = Calc_Gravity(fTimeDelta);
	pOwnerTransform->Move_Direction(1.f, vGravity, 1.f);

	_float3 vPos;
	XMStoreFloat3(&vPos, pOwnerTransform->Get_State(STATE::POSITION));

	///* 현재 프레임의 위치*/
	m_vPosition = PxVec3(vPos.x, vPos.y, vPos.z);

	//무브 자체는 시뮬레이션에서 안 돌아간다고함..

	PxVec3 MoveSum = m_vPosition - m_vPrePosition + vTargetDeltaMove;

	if (false == m_isGravity)
		MoveSum += PxVec3(0.f, -1.f, 0.f)* MoveSum.magnitude() * 0.15f;

	PxControllerFilters ControllerFilter;
	ControllerFilter.mFilterCallback = static_cast<PxQueryFilterCallback*>(m_pQueryFilterCallback);
	// -> 여기서 CCT 필터 추가
	ControllerFilter.mCCTFilterCallback = static_cast<CCTFilterCallback*>(m_pCCTFilterCallback);

	PxExtendedVec3 vPrevPos = m_pController->getFootPosition();

	PxU32 ResultFlag = m_pController->move(MoveSum, 0.001f, fTimeDelta, ControllerFilter);
	
	PxExtendedVec3 vNewPos = m_pController->getFootPosition();
	m_vPrePosition = PxVec3((float)vNewPos.x, (float)vNewPos.y, (float)vNewPos.z);
	pOwnerTransform->Set_State(STATE::POSITION, XMVectorSet((_float)vNewPos.x, (_float)vNewPos.y, (_float)vNewPos.z, 1.f));

	if (ResultFlag & PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		m_fJumpVelocity = 0.f;
		m_fGravityTimeAcc = 0.f;
		m_isGravity = false;
	}

	else
	{
		m_isGravity = true;
	}
}

void CCharacterController::Update_ControllerTransform(_float fTimeDelta, class CTransform* pOwnerTransform)
{
}

void CCharacterController::Set_Position(_vector vPosition)
{
	m_pController->setFootPosition(PxExtendedVec3(XMVectorGetX(vPosition), XMVectorGetY(vPosition), XMVectorGetZ(vPosition)));
}

_vector CCharacterController::Calc_Gravity(_float fTimeDelta)
{
	if (false == m_isGravity && true == m_isRiding)
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);
	else if (false == m_isGravity)
		return XMVectorSet(0.f, -0.2f, 0.f, 0.f);

	m_fJumpVelocity -= m_fGravity * fTimeDelta;

	_float fDeltaY = m_fJumpVelocity * fTimeDelta;

	return XMVectorSet(0.f, fDeltaY, 0.f, 0.f);
}

HRESULT CCharacterController::Ready_CapsuleController(CCT_DESC* pDesc)
{
	m_pMaterial = m_pPxPhysics->createMaterial(pDesc->vMaterial.x, pDesc->vMaterial.y, pDesc->vMaterial.z);
	m_pHitReporter = pDesc->pHitReporter;
	m_pBehaviorCallback = pDesc->pBehaviorCallback;

	PxCapsuleControllerDesc CCTDesc;
	CCTDesc.radius = pDesc->vSize.x / 2.f;      // 반지름
	CCTDesc.height = pDesc->vSize.y;      // 캡슐 높이
	CCTDesc.position = PxExtendedVec3(pDesc->vStartPos.x, pDesc->vStartPos.y, pDesc->vStartPos.z);
	CCTDesc.material = m_pMaterial;    // PxMaterial*
	CCTDesc.contactOffset = 0.3f;                // 충돌 감지 오프셋
	CCTDesc.stepOffset = pDesc->fStepOffset;                // 계단 올라갈 수 있는 높이
	CCTDesc.slopeLimit = cosf(PxPi / 3.f + PxPi / 18.f);      // 오르막 각도 제한
	CCTDesc.density = 10.0f;
	CCTDesc.scaleCoeff = 0.9f;
	CCTDesc.invisibleWallHeight = 0.0f;
	CCTDesc.maxJumpHeight = 0.0f;
	CCTDesc.reportCallback = m_pHitReporter;
	CCTDesc.behaviorCallback = m_pBehaviorCallback;
	CCTDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING;
	CCTDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	CCTDesc.upDirection = PxVec3(0, 1, 0); // 기본 up

	m_tUserData = pDesc->tUserData;
	m_tUserData.pWord0 = &pDesc->iCollisionGroup;
	m_tUserData.pWord1 = &pDesc->iCollisionMask;
	CCTDesc.userData = &m_tUserData;

	if (!CCTDesc.isValid())
		return E_FAIL;

	m_pController = m_pGameInstance->Get_PxCCTManager()->createController(CCTDesc);
	if (nullptr == m_pController)
		return E_FAIL;

	m_pHitReporter->Set_Controller(this, m_pController, pDesc->tUserData);

	m_pQueryFilterCallback = pDesc->pQueryFilterCallback;

	return S_OK;
}

HRESULT CCharacterController::Ready_BoxController(CCT_DESC* pDesc)
{
	m_pMaterial = m_pPxPhysics->createMaterial(pDesc->vMaterial.x, pDesc->vMaterial.y, pDesc->vMaterial.z);
	m_pHitReporter = pDesc->pHitReporter;
	m_pBehaviorCallback = pDesc->pBehaviorCallback;

	PxBoxControllerDesc CCTDesc;
	CCTDesc.halfHeight			= pDesc->vSize.x / 2.0f;
	CCTDesc.halfSideExtent		= pDesc->vSize.y / 2.0f;
	CCTDesc.halfForwardExtent	= pDesc->vSize.z / 2.0f;
	CCTDesc.position			= PxExtendedVec3(pDesc->vStartPos.x, pDesc->vStartPos.y, pDesc->vStartPos.z);
	CCTDesc.material			= m_pMaterial;

	// 선택적 세팅
	CCTDesc.contactOffset		= 0.05f;                // 충돌 감지 오프셋
	CCTDesc.stepOffset			= 0.35f;                // 계단 올라갈 수 있는 높이
	CCTDesc.slopeLimit			= cosf(PxPi / 3.f + PxPi / 18.f);      // 오르막 각도 제한
	CCTDesc.density				= 10.0f;
	CCTDesc.scaleCoeff			= 0.9f;
	CCTDesc.invisibleWallHeight = 0.0f;
	CCTDesc.maxJumpHeight		= 0.0f;
	CCTDesc.reportCallback		= m_pHitReporter;
	CCTDesc.behaviorCallback	= m_pBehaviorCallback;
	CCTDesc.nonWalkableMode		= PxControllerNonWalkableMode::ePREVENT_CLIMBING;
	CCTDesc.upDirection			= PxVec3(0, 1, 0); // 기본 up

	m_tUserData					= pDesc->tUserData;
	CCTDesc.userData			= &m_tUserData;

	if (!CCTDesc.isValid())
		return E_FAIL;

	m_pController = m_pGameInstance->Get_PxCCTManager()->createController(CCTDesc);
	if (nullptr == m_pController)
		return E_FAIL;

	m_pHitReporter->Set_Controller(this, m_pController, pDesc->tUserData);

	return S_OK;
}

CCharacterController* CCharacterController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCharacterController* pInstance = new CCharacterController(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCharacterController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCharacterController::Clone(void* pArg)
{
	CCharacterController* pInstance = new CCharacterController(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCharacterController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCharacterController::Free()
{
	__super::Free();

	if (nullptr != m_pMaterial)
		m_pMaterial->release();

	if (nullptr != m_pController)
	{
		m_pController->release();
		m_pController = nullptr;
	}

	if (nullptr != m_pPxPhysics)
		m_pPxPhysics = nullptr;

	if (nullptr != m_pPxCCTManager)
		m_pPxCCTManager = nullptr;

	Safe_Release(m_pHitReporter);
	Safe_Release(m_pBehaviorCallback);
	Safe_Release(m_pQueryFilterCallback);
	Safe_Release(m_pCCTFilterCallback);
}
