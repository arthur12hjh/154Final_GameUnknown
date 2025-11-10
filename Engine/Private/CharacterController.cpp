#include "CharacterController.h"

#include "GameInstance.h"
#include "CCTHitReporter.h"

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

HRESULT CCharacterController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCharacterController::Initialize(void* pArg)
{
	CCT_DESC* pDesc = static_cast<CCT_DESC*>(pArg);

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

	return S_OK;
}

void CCharacterController::Update_PxPosition(_float fTimeDelta, class CTransform* pOwnerTransform)
{
	m_vPrePosition = PxVec3(m_vPosition.x, m_vPosition.y, m_vPosition.z);

	_float3 vPos;
	XMStoreFloat3(&vPos, pOwnerTransform->Get_State(STATE::POSITION));

	m_vPosition = PxVec3(vPos.x, vPos.y, vPos.z);

	m_pController->move(m_vPosition - m_vPrePosition, 0.001f, fTimeDelta, PxControllerFilters());

	pOwnerTransform->Set_State(STATE::POSITION, XMVectorSet(m_vPosition.x, m_vPosition.y, m_vPosition.z, 1.f));
}

HRESULT CCharacterController::Ready_CapsuleController(CCT_DESC* pDesc)
{
	m_pMaterial = m_pPxPhysics->createMaterial(pDesc->vMaterial.x, pDesc->vMaterial.y, pDesc->vMaterial.z);
	m_pHitReporter = pDesc->pHitReporter;

	PxCapsuleControllerDesc CCTDesc;
	CCTDesc.radius = pDesc->vSize.x / 2.f;      // 반지름
	CCTDesc.height = pDesc->vSize.y;      // 캡슐 높이
	CCTDesc.position = PxExtendedVec3(pDesc->vStartPos.x, pDesc->vStartPos.y, pDesc->vStartPos.z);
	CCTDesc.material = m_pMaterial;    // PxMaterial*

	CCTDesc.contactOffset = 0.05f;                // 충돌 감지 오프셋
	CCTDesc.stepOffset = 0.35f;                // 계단 올라갈 수 있는 높이
	CCTDesc.slopeLimit = cosf(PxPi / 3);      // 오르막 각도 제한
	CCTDesc.density = 10.0f;
	CCTDesc.scaleCoeff = 0.9f;
	CCTDesc.invisibleWallHeight = 0.0f;
	CCTDesc.maxJumpHeight = 0.0f;
	CCTDesc.reportCallback = m_pHitReporter;
	CCTDesc.behaviorCallback = nullptr;
	CCTDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING;
	CCTDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	CCTDesc.upDirection = PxVec3(0, 1, 0); // 기본 up

	m_tUserData = pDesc->tUserData;
	CCTDesc.userData = &m_tUserData;

	if (!CCTDesc.isValid())
		return E_FAIL;

	m_pController = m_pGameInstance->Get_PxCCTManager()->createController(CCTDesc);
	if (nullptr == m_pController)
		return E_FAIL;

	m_pHitReporter->Set_Controller(m_pController, pDesc->tUserData);

	return S_OK;
}

HRESULT CCharacterController::Ready_BoxController(CCT_DESC* pDesc)
{
	m_pMaterial = m_pPxPhysics->createMaterial(pDesc->vMaterial.x, pDesc->vMaterial.y, pDesc->vMaterial.z);
	m_pHitReporter = pDesc->pHitReporter;

	PxBoxControllerDesc CCTDesc;
	CCTDesc.halfHeight			= pDesc->vSize.x / 2.0f;
	CCTDesc.halfSideExtent		= pDesc->vSize.y / 2.0f;
	CCTDesc.halfForwardExtent	= pDesc->vSize.z / 2.0f;
	CCTDesc.position			= PxExtendedVec3(pDesc->vStartPos.x, pDesc->vStartPos.y, pDesc->vStartPos.z);
	CCTDesc.material			= m_pMaterial;

	// 선택적 세팅
	CCTDesc.contactOffset		= 0.05f;                // 충돌 감지 오프셋
	CCTDesc.stepOffset			= 0.35f;                // 계단 올라갈 수 있는 높이
	CCTDesc.slopeLimit			= cosf(PxPi / 3);      // 오르막 각도 제한
	CCTDesc.density				= 10.0f;
	CCTDesc.scaleCoeff			= 0.9f;
	CCTDesc.invisibleWallHeight = 0.0f;
	CCTDesc.maxJumpHeight		= 0.0f;
	CCTDesc.reportCallback		= pDesc->pHitReporter;
	CCTDesc.behaviorCallback	= nullptr;
	CCTDesc.nonWalkableMode		= PxControllerNonWalkableMode::ePREVENT_CLIMBING;
	CCTDesc.upDirection			= PxVec3(0, 1, 0); // 기본 up

	m_tUserData					= pDesc->tUserData;
	CCTDesc.userData			= &m_tUserData;

	if (!CCTDesc.isValid())
		return E_FAIL;

	m_pController = m_pGameInstance->Get_PxCCTManager()->createController(CCTDesc);
	if (nullptr == m_pController)
		return E_FAIL;

	m_pHitReporter->Set_Controller(m_pController, pDesc->tUserData);

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
		m_pController->release();

	if (nullptr != m_pPxPhysics)
		m_pPxPhysics = nullptr;

	if (nullptr != m_pPxCCTManager)
		m_pPxCCTManager = nullptr;

	Safe_Release(m_pHitReporter);
}
