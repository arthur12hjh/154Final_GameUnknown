#include "pch.h"

#include "CinematicModel_Eve.h"
#include "Body_Player.h"
#include "Face_Player.h"
#include "Hair_Player.h"
#include "PonyTail_Player.h"
#include "Weapon.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Effect.h"
#include "Notify.h"

#include "Player.h"
#include "PlayerCCTHitReporter.h"
#include "PlayerBehaviorCallback.h"
#include "PlayerCCTQueryFilterCallback.h"


CCinematicModel_Eve::CCinematicModel_Eve(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter{ pDevice, pContext }
{
}

CCinematicModel_Eve::CCinematicModel_Eve(const CCinematicModel_Eve& Prototype)
	: CCharacter{ Prototype }
{
}

void CCinematicModel_Eve::Active_SFX(const _wstring& strPartTag, const _wstring& strObjectTag, const ANIM_NOTIFY& NotifyReference)
{
	// 어떤 방식으로 짜야하지?
	// 1) strPartTag가 empty일 경우, CinematicModel_Eve에서 탐색
	//    strPartTag가 있을 경우, strPartTag로 모델을 탐색
	// 2) strObjectTag라는 매핑된 값을 문자열 탐색해서 찾고, Play() 함수를 실행함
	// 3) 그 모델들은 시간이 지난 후 알아서 Stop()

	if (strPartTag.empty())
	{

	}
	else
	{
		Find_PartObject(strPartTag)->Active_SFX(strObjectTag, NotifyReference);
	}

}

void CCinematicModel_Eve::Activate_PartObject_Collider(const _wstring& strPartTag, const _wstring& strColliderTag, const ANIM_NOTIFY& NotifyRef)
{
	auto pPartObject = Find_PartObject(strPartTag);
	if (nullptr == pPartObject)
		return;

	pPartObject->Activate_PartObject_Collider(strColliderTag, NotifyRef);
}

HRESULT CCinematicModel_Eve::CallNotify(_uint iNotiType, const AnimNotify* pNotify)
{
	CNotify::NOTIFY_TYPE NotiType = CNotify::NOTIFY_TYPE(iNotiType);
	
	return S_OK;
}

HRESULT CCinematicModel_Eve::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCinematicModel_Eve::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pColliderCom->SetOwner(this);

	return S_OK;
}

void CCinematicModel_Eve::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->Update_PreWorldMatrix();
	m_pCCT->Update_PrePxPosition(m_pTransformCom);

	__super::Priority_Update(fTimeDelta);
}

void CCinematicModel_Eve::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_pColliderCom->UpdateColiision(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CCinematicModel_Eve::Late_Update(_float fTimeDelta)
{
	m_pCCT->Update_PxPosition(fTimeDelta, m_pTransformCom);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	m_pGameInstance->ADD_Collider(m_pColliderCom);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	__super::Late_Update(fTimeDelta);
}

HRESULT CCinematicModel_Eve::Render()
{
	return S_OK;
}

HRESULT CCinematicModel_Eve::Ready_Components()
{
	/* Com_Collider_AABB */
	CBoxCollider::BOX_COLLIDER_DESC		AABBDesc{};
	AABBDesc.vSize = _float3(1.5f, 2.f, 1.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;

	/* Com_CCT */
	CCharacterController::CCT_DESC Desc;
	PxUserData tUserData;
	tUserData.szActorTag = TEXT("CinematicModel_Eve_CCT");

	Desc.eCharacterControllerType = CCharacterController::CCT_SHAPE::CAPSULE;
	Desc.tUserData = tUserData;
	Desc.vSize = _float3(2.5f, 2.5f, 0.f);
	XMStoreFloat4(&Desc.vStartPos, m_pTransformCom->Get_State(STATE::POSITION));
	Desc.vMaterial = _float3(0.5f, 0.5f, 0.f);
	Desc.pHitReporter = CPlayerCCTHitReporter::Create();
	Desc.pBehaviorCallback = CPlayerBehaviorCallback::Create();
	Desc.pQueryFilterCallback = CPlayerCCTQueryFilterCallback::Create();
	Desc.iCollisionGroup = PHYSX_CCT;
	Desc.iCollisionMask &= ~(PHYSX_CUSTOM_3);

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterController"),
		TEXT("Com_CCT"), reinterpret_cast<CComponent**>(&m_pCCT), &Desc)))
		return E_FAIL;

	m_pGameInstance->Add_CCT_ToPhysx(this, m_pCCT);

	return S_OK;
}

HRESULT CCinematicModel_Eve::Ready_PartObjects()
{
	CBody_Player::BODY_PLAYER_DESC BodyDesc{};
	BodyDesc.pParentTransform = m_pTransformCom;

	/* Part_Body */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	CBody_Player* pBody = dynamic_cast<CBody_Player*>(Find_PartObject(TEXT("Part_Body")));

	CWeapon::WEAPON_DESC	WeaponDesc{};
	WeaponDesc.pParent = this;
	WeaponDesc.pSocketMatrix = pBody->Get_BoneMatrixPtr("Weapon");
	WeaponDesc.pParentTransform = m_pTransformCom;

	/* Part_Weapon */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon"),
		TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;

	m_pWeapon = static_cast<CWeapon*>(Find_PartObject(TEXT("Part_Weapon")));

	CFace_Player::FACE_PLAYER_DESC FaceDesc{};
	FaceDesc.pParentTransform = m_pTransformCom;
	FaceDesc.pBodyPtr = pBody;

	/* Part_Face */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Face_Player"),
		TEXT("Part_Face"), &FaceDesc)))
		return E_FAIL;

	CHair_Player::HAIR_PLAYER_DESC HairDesc{};
	HairDesc.pParentTransform = m_pTransformCom;
	HairDesc.pBodyPtr = pBody;

	/* Part_Hair */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Hair_Player"),
		TEXT("Part_Hair"), &HairDesc)))
		return E_FAIL;

	CPonyTail_Player::PONYTAIL_PLAYER_DESC PonyTailDesc{};
	PonyTailDesc.pParentTransform = m_pTransformCom;
	PonyTailDesc.pBodyPtr = pBody;

	/* Part_PonyTail */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_PonyTail_Player"),
		TEXT("Part_PonyTail"), &PonyTailDesc)))
		return E_FAIL;

	Import_ModelPtr();
	m_pNotifyCom->Set_ModelCom(m_pBodyModelCom);

	return S_OK;
}

CCinematicModel_Eve* CCinematicModel_Eve::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCinematicModel_Eve* pInstance = new CCinematicModel_Eve(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCinematicModel_Eve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinematicModel_Eve::Clone(void* pArg)
{
	CCinematicModel_Eve* pInstance = new CCinematicModel_Eve(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCinematicModel_Eve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinematicModel_Eve::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
